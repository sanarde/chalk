// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QByteArray>
#include <QVariantMap>

namespace Chalk::PanelProperties {

inline constexpr const char *WindowProperty = "chalkPanel";
inline constexpr const char *ShellKey = "chalk-layer-shell";

enum Layer { Background = 0, Bottom = 1, Top = 2, Overlay = 3 };

enum Anchor { AnchorTop = 1, AnchorBottom = 2, AnchorLeft = 4, AnchorRight = 8, AnchorAll = 15 };

enum Keyboard { KeyboardNone = 0, KeyboardExclusive = 1, KeyboardOnDemand = 2 };

struct Panel {
    int layer = Top;
    int anchors = 0;
    int width = 0;
    int height = 0;
    int exclusive = 0;
    int keyboard = KeyboardNone;
    int marginTop = 0;
    int marginRight = 0;
    int marginBottom = 0;
    int marginLeft = 0;

    bool operator==(const Panel &other) const = default;
};

QVariantMap toMap(const Panel &panel);
Panel fromMap(const QVariantMap &map);

}
