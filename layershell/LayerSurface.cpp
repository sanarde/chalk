// SPDX-License-Identifier: GPL-3.0-only

#include "LayerSurface.h"

#include <QWindow>

namespace Chalk {

using namespace PanelProperties;

LayerSurface::LayerSurface(QtWayland::zwlr_layer_shell_v1 *shell, QtWaylandClient::QWaylandWindow *window,
                           const Panel &panel)
    : QWaylandShellSurface(window)
{
    init(shell->get_layer_surface(wlSurface(), nullptr, static_cast<uint32_t>(panel.layer),
                                  QStringLiteral("chalk")));

    set_anchor(static_cast<uint32_t>(panel.anchors));
    set_size(static_cast<uint32_t>(panel.width), static_cast<uint32_t>(panel.height));
    set_exclusive_zone(panel.exclusive);
    set_margin(panel.marginTop, panel.marginRight, panel.marginBottom, panel.marginLeft);
    set_keyboard_interactivity(static_cast<uint32_t>(panel.keyboard));
}

LayerSurface::~LayerSurface()
{
    destroy();
}

bool LayerSurface::isExposed() const
{
    return m_configured;
}

void LayerSurface::applyConfigure()
{
    if (m_pending.isValid())
        resizeFromApplyConfigure(m_pending);
}

void LayerSurface::setWindowGeometry(const QRect &rect)
{
    set_size(static_cast<uint32_t>(rect.width()), static_cast<uint32_t>(rect.height()));
    window()->commit();
}

void LayerSurface::zwlr_layer_surface_v1_configure(uint32_t serial, uint32_t width, uint32_t height)
{
    ack_configure(serial);
    m_pending = QSize(static_cast<int>(width), static_cast<int>(height));

    const bool first = !m_configured;
    m_configured = true;
    if (first) {
        resizeFromApplyConfigure(m_pending);
        window()->updateExposure();
        return;
    }
    applyConfigureWhenPossible();
}

void LayerSurface::zwlr_layer_surface_v1_closed()
{
    window()->window()->close();
}

}
