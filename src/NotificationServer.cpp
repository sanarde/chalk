// SPDX-License-Identifier: GPL-3.0-only

#include "NotificationServer.h"

#include "NotificationStore.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

namespace Chalk {

namespace {

constexpr const char *ServiceName = "org.freedesktop.Notifications";
constexpr const char *ObjectPath = "/org/freedesktop/Notifications";

}

NotificationServer::NotificationServer(QObject *parent)
    : QObject(parent)
{
}

NotificationStore *NotificationServer::store() const
{
    return m_store;
}

void NotificationServer::setStore(NotificationStore *store)
{
    if (m_store == store)
        return;
    if (m_store)
        m_store->disconnect(this);
    m_store = store;
    if (m_store) {
        connect(m_store, &NotificationStore::closed, this, &NotificationServer::notificationClosed);
        connect(m_store, &NotificationStore::actionInvoked, this, &NotificationServer::actionInvoked);
    }
    emit storeChanged();
}

bool NotificationServer::active() const
{
    return m_active;
}

bool NotificationServer::start()
{
    if (m_active)
        return true;
    if (!m_store)
        return false;

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        qWarning() << "notifications: there is no session bus";
        return false;
    }

    new NotificationsAdaptor(this);
    if (!bus.registerObject(QLatin1String(ObjectPath), this)) {
        qWarning() << "notifications: cannot register the object" << bus.lastError().message();
        return false;
    }
    if (!bus.registerService(QLatin1String(ServiceName))) {
        qWarning() << "notifications: another daemon owns" << ServiceName;
        bus.unregisterObject(QLatin1String(ObjectPath));
        return false;
    }

    m_active = true;
    emit activeChanged();
    return true;
}

uint NotificationServer::notify(const QString &app, uint replacesId, const QString &summary, const QString &body,
                                const QStringList &actions, const QVariantMap &hints, int expireTimeout)
{
    if (!m_store)
        return 0;
    return m_store->notify(app, replacesId, summary, body, actions, hints, expireTimeout);
}

void NotificationServer::close(uint id)
{
    if (m_store)
        m_store->close(id, NotificationStore::ClosedByCall);
}

NotificationsAdaptor::NotificationsAdaptor(NotificationServer *server)
    : QDBusAbstractAdaptor(server)
    , m_server(server)
{
    connect(server, &NotificationServer::notificationClosed, this, &NotificationsAdaptor::NotificationClosed);
    connect(server, &NotificationServer::actionInvoked, this, &NotificationsAdaptor::ActionInvoked);
}

QStringList NotificationsAdaptor::GetCapabilities()
{
    return {QStringLiteral("body"), QStringLiteral("body-markup"), QStringLiteral("actions")};
}

uint NotificationsAdaptor::Notify(const QString &app_name, uint replaces_id, const QString &, const QString &summary,
                                  const QString &body, const QStringList &actions, const QVariantMap &hints,
                                  int expire_timeout)
{
    return m_server->notify(app_name, replaces_id, summary, body, actions, hints, expire_timeout);
}

void NotificationsAdaptor::CloseNotification(uint id)
{
    m_server->close(id);
}

QString NotificationsAdaptor::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    vendor = QStringLiteral("SanarDE");
    version = QCoreApplication::applicationVersion();
    spec_version = QStringLiteral("1.2");
    return QStringLiteral("Chalk");
}

}
