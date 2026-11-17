// SPDX-License-Identifier: GPL-3.0-only

#include "NotificationStore.h"

#include <QLocale>

namespace Chalk {

NotificationStore::NotificationStore(QObject *parent)
    : QAbstractListModel(parent)
{
    m_bannerTimer.setSingleShot(true);
    connect(&m_bannerTimer, &QTimer::timeout, this, &NotificationStore::expireBanner);
}

int NotificationStore::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_entries.size());
}

QVariant NotificationStore::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return {};

    const Entry &entry = m_entries.at(index.row());
    switch (role) {
    case IdRole:
        return entry.id;
    case AppRole:
        return entry.app;
    case SummaryRole:
        return entry.summary;
    case BodyRole:
        return entry.body;
    case UrgencyRole:
        return entry.urgency;
    case TimeRole:
        return QLocale().toString(entry.time.time(), QStringLiteral("HH:mm"));
    case ActionsRole:
        return entry.actions;
    case LiveRole:
        return entry.live;
    default:
        return {};
    }
}

QHash<int, QByteArray> NotificationStore::roleNames() const
{
    return {
        {IdRole, "id"},
        {AppRole, "app"},
        {SummaryRole, "summary"},
        {BodyRole, "body"},
        {UrgencyRole, "urgency"},
        {TimeRole, "time"},
        {ActionsRole, "actions"},
        {LiveRole, "live"},
    };
}

int NotificationStore::count() const
{
    return static_cast<int>(m_entries.size());
}

QVariantMap NotificationStore::banner() const
{
    const int row = indexOf(m_bannerId);
    if (m_bannerId == 0 || row < 0)
        return {};

    const Entry &entry = m_entries.at(row);
    return {
        {QStringLiteral("id"), entry.id},
        {QStringLiteral("app"), entry.app},
        {QStringLiteral("summary"), entry.summary},
        {QStringLiteral("body"), entry.body},
        {QStringLiteral("urgency"), entry.urgency},
        {QStringLiteral("actions"), entry.actions},
    };
}

bool NotificationStore::doNotDisturb() const
{
    return m_doNotDisturb;
}

void NotificationStore::setDoNotDisturb(bool enabled)
{
    if (m_doNotDisturb == enabled)
        return;
    m_doNotDisturb = enabled;
    if (enabled)
        hideBanner();
    emit doNotDisturbChanged();
}

int NotificationStore::indexOf(uint id) const
{
    for (int row = 0; row < m_entries.size(); ++row) {
        if (m_entries.at(row).id == id)
            return row;
    }
    return -1;
}

int NotificationStore::bannerTimeout(int expireTimeout, int urgency)
{
    if (urgency >= CriticalUrgency || expireTimeout == 0)
        return 0;
    return expireTimeout < 0 ? DefaultTimeout : expireTimeout;
}

QStringList NotificationStore::sanitizedActions(const QStringList &actions)
{
    QStringList result;
    for (qsizetype index = 0; index + 1 < actions.size() && result.size() < 6; index += 2) {
        if (actions.at(index).isEmpty())
            continue;
        result << actions.at(index) << actions.at(index + 1).left(40);
    }
    return result;
}

uint NotificationStore::notify(const QString &app, uint replacesId, const QString &summary, const QString &body,
                               const QStringList &actions, const QVariantMap &hints, int expireTimeout)
{
    Entry entry;
    entry.app = app.left(100);
    entry.summary = summary.left(MaxText);
    entry.body = body.left(MaxText);
    entry.urgency = qBound(0, hints.value(QStringLiteral("urgency"), 1).toInt(), 2);
    entry.time = QDateTime::currentDateTime();
    entry.actions = sanitizedActions(actions);

    const int existing = replacesId != 0 ? indexOf(replacesId) : -1;
    if (existing >= 0) {
        entry.id = replacesId;
        if (existing == 0) {
            m_entries[0] = entry;
            emit dataChanged(index(0), index(0));
        } else {
            beginMoveRows({}, existing, existing, {}, 0);
            m_entries.move(existing, 0);
            endMoveRows();
            m_entries[0] = entry;
            emit dataChanged(index(0), index(0));
        }
    } else {
        entry.id = m_nextId++;
        beginInsertRows({}, 0, 0);
        m_entries.prepend(entry);
        endInsertRows();

        while (m_entries.size() > MaxEntries) {
            const int last = static_cast<int>(m_entries.size()) - 1;
            const uint dropped = m_entries.at(last).id;
            const bool live = m_entries.at(last).live;
            beginRemoveRows({}, last, last);
            m_entries.removeLast();
            endRemoveRows();
            if (dropped == m_bannerId)
                clearBanner();
            if (live)
                emit closed(dropped, Undefined);
        }
        emit countChanged();
    }

    if (m_doNotDisturb && entry.urgency < CriticalUrgency) {
        const int row = indexOf(entry.id);
        if (row >= 0) {
            m_entries[row].live = false;
            emit dataChanged(index(row), index(row), {LiveRole});
        }
        emit closed(entry.id, Undefined);
        return entry.id;
    }

    showBanner(entry, bannerTimeout(expireTimeout, entry.urgency));
    return entry.id;
}

void NotificationStore::showBanner(const Entry &entry, int timeout)
{
    m_bannerId = entry.id;
    if (timeout > 0)
        m_bannerTimer.start(timeout);
    else
        m_bannerTimer.stop();
    emit bannerChanged();
}

void NotificationStore::clearBanner()
{
    m_bannerTimer.stop();
    if (m_bannerId == 0)
        return;
    m_bannerId = 0;
    emit bannerChanged();
}

void NotificationStore::expireBanner()
{
    const uint id = m_bannerId;
    clearBanner();
    const int row = indexOf(id);
    if (row < 0 || !m_entries.at(row).live)
        return;
    m_entries[row].live = false;
    emit dataChanged(index(row), index(row), {LiveRole});
    emit closed(id, Expired);
}

void NotificationStore::hideBanner()
{
    const uint id = m_bannerId;
    clearBanner();
    const int row = indexOf(id);
    if (row < 0 || !m_entries.at(row).live)
        return;
    m_entries[row].live = false;
    emit dataChanged(index(row), index(row), {LiveRole});
    emit closed(id, Dismissed);
}

void NotificationStore::close(uint id, CloseReason reason)
{
    const int row = indexOf(id);
    if (row < 0)
        return;

    const bool live = m_entries.at(row).live;
    if (id == m_bannerId)
        clearBanner();
    beginRemoveRows({}, row, row);
    m_entries.removeAt(row);
    endRemoveRows();
    emit countChanged();
    if (live)
        emit closed(id, reason);
}

void NotificationStore::remove(uint id)
{
    close(id, Dismissed);
}

void NotificationStore::clear()
{
    clearBanner();
    if (m_entries.isEmpty())
        return;

    QList<uint> live;
    for (const Entry &entry : std::as_const(m_entries)) {
        if (entry.live)
            live << entry.id;
    }
    beginResetModel();
    m_entries.clear();
    endResetModel();
    emit countChanged();
    for (const uint id : live)
        emit closed(id, Dismissed);
}

void NotificationStore::invoke(uint id, const QString &key)
{
    const int row = indexOf(id);
    if (row < 0 || !m_entries.at(row).live)
        return;
    const QStringList &actions = m_entries.at(row).actions;
    bool known = false;
    for (qsizetype index = 0; index < actions.size(); index += 2)
        known = known || actions.at(index) == key;
    if (!known)
        return;

    emit actionInvoked(id, key);
    hideBanner();
}

}
