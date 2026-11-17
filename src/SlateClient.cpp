// SPDX-License-Identifier: GPL-3.0-only

#include "SlateClient.h"

#include <algorithm>

#include <QDir>
#include <QVariantMap>

namespace Chalk {

namespace {

constexpr int ReconnectMs = 1000;

QByteArray command(const QByteArray &text)
{
    return text + '\n';
}

}

SlateClient::SlateClient(QObject *parent)
    : QObject(parent)
{
    m_reconnect.setSingleShot(true);
    m_reconnect.setInterval(ReconnectMs);
    m_refresh.setSingleShot(true);
    m_refresh.setInterval(0);

    connect(&m_reconnect, &QTimer::timeout, this, &SlateClient::connectToSlate);
    connect(&m_refresh, &QTimer::timeout, this, &SlateClient::requestState);
    connect(&m_control, &QLocalSocket::connected, this, &SlateClient::onConnected);
    connect(&m_events, &QLocalSocket::connected, this, &SlateClient::onConnected);
    connect(&m_control, &QLocalSocket::disconnected, this, &SlateClient::onDisconnected);
    connect(&m_events, &QLocalSocket::disconnected, this, &SlateClient::onDisconnected);
    connect(&m_control, &QLocalSocket::readyRead, this, &SlateClient::readReplies);
    connect(&m_events, &QLocalSocket::readyRead, this, &SlateClient::readEvents);
    connect(&m_control, &QLocalSocket::errorOccurred, this, &SlateClient::onDisconnected);
    connect(&m_events, &QLocalSocket::errorOccurred, this, &SlateClient::onDisconnected);

    connectToSlate();
}

QString SlateClient::socketPath()
{
    const QString explicitPath = qEnvironmentVariable("SLATE_SOCKET");
    if (!explicitPath.isEmpty())
        return explicitPath;

    const QString runtime = qEnvironmentVariable("XDG_RUNTIME_DIR", QDir::tempPath());
    return runtime + QStringLiteral("/slate.sock");
}

bool SlateClient::connected() const
{
    return m_connected;
}

int SlateClient::workspace() const
{
    return m_state.workspace;
}

QVariantList SlateClient::workspaces() const
{
    QVariantList list;
    for (const WorkspaceInfo &workspace : m_state.workspaces) {
        list.append(QVariantMap{
            {"index", workspace.index},
            {"windows", workspace.windows},
            {"active", workspace.active},
        });
    }
    return list;
}

QString SlateClient::focusedTitle() const
{
    return m_state.focusedTitle;
}

QString SlateClient::focusedApp() const
{
    return appDisplayName(m_state.focusedAppId, m_state.focusedTitle);
}

QVariantList SlateClient::windows() const
{
    QList<WindowInfo> ordered = m_state.windows;
    std::sort(ordered.begin(), ordered.end(), [](const WindowInfo &a, const WindowInfo &b) { return a.id < b.id; });
    QVariantList out;
    for (const WindowInfo &window : ordered) {
        if (window.workspace != m_state.workspace)
            continue;
        out.append(QVariantMap{
            {"id", window.id},
            {"title", window.title},
            {"app", appDisplayName(window.appId, window.title)},
            {"focused", window.focused},
            {"minimized", window.minimized},
        });
    }
    return out;
}

void SlateClient::activateWindow(qulonglong id)
{
    if (m_control.state() != QLocalSocket::ConnectedState)
        return;
    for (const WindowInfo &window : m_state.windows) {
        if (window.id != id)
            continue;
        if (window.focused && !window.minimized) {
            m_control.write(command("minimize " + QByteArray::number(id)));
        } else {
            if (window.minimized)
                m_control.write(command("restore " + QByteArray::number(id)));
            m_control.write(command("focus " + QByteArray::number(id)));
        }
        return;
    }
}

void SlateClient::switchWorkspace(int index)
{
    action(QStringLiteral("workspace %1").arg(index));
}

void SlateClient::action(const QString &action)
{
    if (m_control.state() == QLocalSocket::ConnectedState)
        m_control.write(command("action " + action.toUtf8()));
}

void SlateClient::connectToSlate()
{
    m_replies.clear();
    m_eventBuffer.clear();
    m_acknowledged = false;
    m_resetting = true;
    m_control.abort();
    m_events.abort();
    m_resetting = false;

    const QString path = socketPath();
    m_control.connectToServer(path);
    m_events.connectToServer(path);
}

void SlateClient::onConnected()
{
    if (sender() == &m_events)
        m_events.write(command("subscribe"));

    const bool both = m_control.state() == QLocalSocket::ConnectedState
        && m_events.state() == QLocalSocket::ConnectedState;
    if (both && !m_connected) {
        m_connected = true;
        emit changed();
        requestState();
    }
}

void SlateClient::onDisconnected()
{
    if (m_resetting || m_reconnect.isActive())
        return;

    m_reconnect.start();
    m_control.abort();
    m_events.abort();
    if (m_connected || m_state.valid) {
        m_connected = false;
        m_state = {};
        emit changed();
    }
}

void SlateClient::requestState()
{
    if (m_control.state() == QLocalSocket::ConnectedState)
        m_control.write(command("state"));
}

void SlateClient::readReplies()
{
    m_replies += m_control.readAll();
    qsizetype end;
    while ((end = m_replies.indexOf('\n')) >= 0) {
        const QByteArray line = m_replies.left(end);
        m_replies.remove(0, end + 1);

        const SlateState state = parseState(line);
        if (state.valid)
            apply(state);
    }
}

void SlateClient::readEvents()
{
    m_eventBuffer += m_events.readAll();
    qsizetype end;
    bool event = false;
    while ((end = m_eventBuffer.indexOf('\n')) >= 0) {
        m_eventBuffer.remove(0, end + 1);
        if (!m_acknowledged)
            m_acknowledged = true;
        else
            event = true;
    }
    if (event)
        m_refresh.start();
}

void SlateClient::apply(const SlateState &state)
{
    m_state = state;
    emit changed();
}

}
