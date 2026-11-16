// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtWaylandClient/private/qwaylandshellintegrationplugin_p.h>

namespace Chalk {

class LayerShellPlugin : public QtWaylandClient::QWaylandShellIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWaylandShellIntegrationFactoryInterface_iid FILE "chalk-layer-shell.json")

public:
    QtWaylandClient::QWaylandShellIntegration *create(const QString &key, const QStringList &parameters) override;
};

}
