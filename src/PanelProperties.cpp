// SPDX-License-Identifier: GPL-3.0-only

#include "PanelProperties.h"

namespace Chalk::PanelProperties {

QVariantMap toMap(const Panel &panel)
{
    return {
        {"layer", panel.layer},
        {"anchors", panel.anchors},
        {"width", panel.width},
        {"height", panel.height},
        {"exclusive", panel.exclusive},
        {"keyboard", panel.keyboard},
        {"marginTop", panel.marginTop},
        {"marginRight", panel.marginRight},
        {"marginBottom", panel.marginBottom},
        {"marginLeft", panel.marginLeft},
    };
}

Panel fromMap(const QVariantMap &map)
{
    Panel panel;
    panel.layer = map.value("layer", panel.layer).toInt();
    panel.anchors = map.value("anchors", panel.anchors).toInt();
    panel.width = map.value("width", panel.width).toInt();
    panel.height = map.value("height", panel.height).toInt();
    panel.exclusive = map.value("exclusive", panel.exclusive).toInt();
    panel.keyboard = map.value("keyboard", panel.keyboard).toInt();
    panel.marginTop = map.value("marginTop", panel.marginTop).toInt();
    panel.marginRight = map.value("marginRight", panel.marginRight).toInt();
    panel.marginBottom = map.value("marginBottom", panel.marginBottom).toInt();
    panel.marginLeft = map.value("marginLeft", panel.marginLeft).toInt();
    return panel;
}

}
