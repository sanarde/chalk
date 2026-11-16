// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QSize>

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include "PanelProperties.h"
#include "qwayland-wlr-layer-shell-unstable-v1.h"

namespace Chalk {

class LayerSurface : public QtWaylandClient::QWaylandShellSurface, public QtWayland::zwlr_layer_surface_v1
{
    Q_OBJECT

public:
    LayerSurface(QtWayland::zwlr_layer_shell_v1 *shell, QtWaylandClient::QWaylandWindow *window,
                 const PanelProperties::Panel &panel);
    ~LayerSurface() override;

    bool isExposed() const override;
    void applyConfigure() override;
    void setWindowGeometry(const QRect &rect) override;

protected:
    void zwlr_layer_surface_v1_configure(uint32_t serial, uint32_t width, uint32_t height) override;
    void zwlr_layer_surface_v1_closed() override;

private:
    QSize m_pending;
    bool m_configured = false;
};

}
