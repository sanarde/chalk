// SPDX-License-Identifier: GPL-3.0-only

#include "LayerShell.h"

#include <QWindow>

#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include "LayerSurface.h"
#include "PanelProperties.h"

namespace Chalk {

namespace {

constexpr int ProtocolVersion = 4;

}

LayerShell::LayerShell()
    : QWaylandShellIntegrationTemplate(ProtocolVersion)
{
}

QtWaylandClient::QWaylandShellSurface *LayerShell::createShellSurface(QtWaylandClient::QWaylandWindow *window)
{
    const QVariant requested = window->window()->property(PanelProperties::WindowProperty);
    const PanelProperties::Panel panel = PanelProperties::fromMap(requested.toMap());
    return new LayerSurface(this, window, panel);
}

}
