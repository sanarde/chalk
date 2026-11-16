// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QByteArray>
#include <QLocalSocket>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include "SlateState.h"

namespace Chalk {

class SlateClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool connected READ connected NOTIFY changed FINAL)
    Q_PROPERTY(int workspace READ workspace NOTIFY changed FINAL)
    Q_PROPERTY(QVariantList workspaces READ workspaces NOTIFY changed FINAL)
    Q_PROPERTY(QString focusedTitle READ focusedTitle NOTIFY changed FINAL)
    Q_PROPERTY(QString focusedApp READ focusedApp NOTIFY changed FINAL)

public:
    explicit SlateClient(QObject *parent = nullptr);

    bool connected() const;
    int workspace() const;
    QVariantList workspaces() const;
    QString focusedTitle() const;
    QString focusedApp() const;

    Q_INVOKABLE void switchWorkspace(int index);
    Q_INVOKABLE void action(const QString &action);

    static QString socketPath();

signals:
    void changed();

private:
    void connectToSlate();
    void onConnected();
    void onDisconnected();
    void requestState();
    void readReplies();
    void readEvents();
    void apply(const SlateState &state);

    QLocalSocket m_control;
    QLocalSocket m_events;
    QTimer m_reconnect;
    QTimer m_refresh;
    QByteArray m_replies;
    QByteArray m_eventBuffer;
    bool m_acknowledged = false;
    bool m_resetting = false;
    bool m_connected = false;
    SlateState m_state;
};

}
