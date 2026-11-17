// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QStringList>
#include <QTimer>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

namespace Chalk {

class NotificationStore : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(QVariantMap banner READ banner NOTIFY bannerChanged FINAL)
    Q_PROPERTY(bool doNotDisturb READ doNotDisturb WRITE setDoNotDisturb NOTIFY doNotDisturbChanged FINAL)

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        AppRole,
        SummaryRole,
        BodyRole,
        UrgencyRole,
        TimeRole,
        ActionsRole,
        LiveRole,
    };

    enum CloseReason : uint { Expired = 1, Dismissed = 2, ClosedByCall = 3, Undefined = 4 };

    static constexpr int MaxEntries = 50;
    static constexpr int MaxText = 2000;
    static constexpr int DefaultTimeout = 5000;
    static constexpr int CriticalUrgency = 2;

    explicit NotificationStore(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    QVariantMap banner() const;
    bool doNotDisturb() const;
    void setDoNotDisturb(bool enabled);

    uint notify(const QString &app, uint replacesId, const QString &summary, const QString &body,
                const QStringList &actions, const QVariantMap &hints, int expireTimeout);
    void close(uint id, CloseReason reason);

    Q_INVOKABLE void hideBanner();
    Q_INVOKABLE void remove(uint id);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void invoke(uint id, const QString &key);

    static int bannerTimeout(int expireTimeout, int urgency);
    static QStringList sanitizedActions(const QStringList &actions);

signals:
    void countChanged();
    void bannerChanged();
    void doNotDisturbChanged();
    void closed(uint id, uint reason);
    void actionInvoked(uint id, const QString &key);

private:
    struct Entry {
        uint id = 0;
        QString app;
        QString summary;
        QString body;
        int urgency = 1;
        QDateTime time;
        QStringList actions;
        bool live = true;
    };

    int indexOf(uint id) const;
    void showBanner(const Entry &entry, int timeout);
    void clearBanner();
    void expireBanner();

    QList<Entry> m_entries;
    uint m_nextId = 1;
    uint m_bannerId = 0;
    bool m_doNotDisturb = false;
    QTimer m_bannerTimer;
};

}
