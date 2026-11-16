// SPDX-License-Identifier: GPL-3.0-only

#include "PanelShell.h"

#include "PanelProperties.h"

#include <QGuiApplication>
#include <QScreen>

#include <xcb/xcb.h>

namespace Chalk {

namespace {

constexpr int StrutValues = 12;

xcb_atom_t atom(xcb_connection_t *connection, const char *name)
{
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 0, static_cast<uint16_t>(qstrlen(name)), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, nullptr);
    if (!reply)
        return XCB_ATOM_NONE;
    const xcb_atom_t result = reply->atom;
    free(reply);
    return result;
}

}

PanelShell::PanelShell(QObject *parent)
    : QObject(parent)
{
}

QWindow *PanelShell::window() const
{
    return m_window;
}

void PanelShell::setWindow(QWindow *window)
{
    if (m_window == window)
        return;
    m_window = window;
    emit windowChanged();
}

PanelShell::Edge PanelShell::edge() const
{
    return m_edge;
}

void PanelShell::setEdge(Edge edge)
{
    if (m_edge == edge)
        return;
    m_edge = edge;
    emit edgeChanged();
}

int PanelShell::thickness() const
{
    return m_thickness;
}

void PanelShell::setThickness(int thickness)
{
    if (m_thickness == thickness)
        return;
    m_thickness = thickness;
    emit thicknessChanged();
}

PanelShell::Kind PanelShell::kind() const
{
    return m_kind;
}

void PanelShell::setKind(Kind kind)
{
    if (m_kind == kind)
        return;
    m_kind = kind;
    emit kindChanged();
}

void PanelShell::show()
{
    if (!m_window)
        return;

    QScreen *screen = m_window->screen() ? m_window->screen() : QGuiApplication::primaryScreen();
    const QRect area = screen->geometry();

    m_window->setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus
                       | Qt::WindowStaysOnTopHint);

    const bool wayland = QGuiApplication::platformName() == QLatin1String("wayland");
    if (m_kind == Overlay) {
        m_window->setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        m_window->setGeometry(wayland ? QRect(0, 0, 1, 1) : area);
    } else if (wayland) {
        m_window->setGeometry(0, 0, 1, m_thickness);
    } else {
        const int y = m_edge == Top ? area.top() : area.bottom() + 1 - m_thickness;
        m_window->setGeometry(area.left(), y, area.width(), m_thickness);
    }

    if (QGuiApplication::platformName() == QLatin1String("xcb") && m_kind == Panel)
        placeOnX11();
    else if (wayland)
        placeOnWayland();

    m_window->show();
}

void PanelShell::placeOnWayland()
{
    PanelProperties::Panel panel;
    if (m_kind == Overlay) {
        panel.layer = PanelProperties::Overlay;
        panel.anchors = PanelProperties::AnchorAll;
        panel.exclusive = -1;
        panel.keyboard = PanelProperties::KeyboardOnDemand;
    } else {
        panel.layer = PanelProperties::Top;
        panel.anchors = (m_edge == Top ? PanelProperties::AnchorTop : PanelProperties::AnchorBottom)
            | PanelProperties::AnchorLeft | PanelProperties::AnchorRight;
        panel.height = m_thickness;
        panel.exclusive = m_thickness;
    }
    m_window->setProperty(PanelProperties::WindowProperty, PanelProperties::toMap(panel));
}

void PanelShell::placeOnX11()
{
    auto *x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11)
        return;

    xcb_connection_t *connection = x11->connection();
    const xcb_window_t id = static_cast<xcb_window_t>(m_window->winId());

    const xcb_atom_t type = atom(connection, "_NET_WM_WINDOW_TYPE");
    const xcb_atom_t dock = atom(connection, "_NET_WM_WINDOW_TYPE_DOCK");
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, id, type, XCB_ATOM_ATOM, 32, 1, &dock);

    const QRect geometry = m_window->geometry();
    uint32_t strut[StrutValues] = {};
    if (m_edge == Top) {
        strut[2] = static_cast<uint32_t>(m_thickness);
        strut[8] = static_cast<uint32_t>(geometry.left());
        strut[9] = static_cast<uint32_t>(geometry.right());
    } else {
        strut[3] = static_cast<uint32_t>(m_thickness);
        strut[10] = static_cast<uint32_t>(geometry.left());
        strut[11] = static_cast<uint32_t>(geometry.right());
    }
    const xcb_atom_t partial = atom(connection, "_NET_WM_STRUT_PARTIAL");
    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, id, partial, XCB_ATOM_CARDINAL, 32, StrutValues, strut);
    xcb_flush(connection);
}

}
