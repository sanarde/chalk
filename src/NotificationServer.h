// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include "NotificationStore.h"

#include <QDBusAbstractAdaptor>
#include <QObject>
#include <QPointer>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

namespace Chalk {

class NotificationServer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(NotificationStore *store READ store WRITE setStore NOTIFY storeChanged FINAL)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged FINAL)

public:
    explicit NotificationServer(QObject *parent = nullptr);

    NotificationStore *store() const;
    void setStore(NotificationStore *store);
    bool active() const;

    Q_INVOKABLE bool start();

    uint notify(const QString &app, uint replacesId, const QString &summary, const QString &body,
                const QStringList &actions, const QVariantMap &hints, int expireTimeout);
    void close(uint id);

signals:
    void storeChanged();
    void activeChanged();
    void notificationClosed(uint id, uint reason);
    void actionInvoked(uint id, const QString &key);

private:
    QPointer<NotificationStore> m_store;
    bool m_active = false;
};

class NotificationsAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
    explicit NotificationsAdaptor(NotificationServer *server);

public slots:
    QStringList GetCapabilities();
    uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary,
                const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout);
    void CloseNotification(uint id);
    QString GetServerInformation(QString &vendor, QString &version, QString &spec_version);

signals:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &action_key);

private:
    NotificationServer *m_server;
};

}
