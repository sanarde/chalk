// SPDX-License-Identifier: GPL-3.0-only

#include "Plugin.h"

#include "LayerShell.h"
#include "PanelProperties.h"

namespace Chalk {

QtWaylandClient::QWaylandShellIntegration *LayerShellPlugin::create(const QString &key, const QStringList &)
{
    if (key == QLatin1String(PanelProperties::ShellKey))
        return new LayerShell;
    return nullptr;
}

}
