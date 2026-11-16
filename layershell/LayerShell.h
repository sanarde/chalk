// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>

#include "qwayland-wlr-layer-shell-unstable-v1.h"

namespace Chalk {

class LayerShell : public QtWaylandClient::QWaylandShellIntegrationTemplate<LayerShell>,
                   public QtWayland::zwlr_layer_shell_v1
{
public:
    LayerShell();

    QtWaylandClient::QWaylandShellSurface *createShellSurface(QtWaylandClient::QWaylandWindow *window) override;
};

}
