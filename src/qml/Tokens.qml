// SPDX-License-Identifier: GPL-3.0-only

pragma Singleton

import QtQuick

QtObject {
    readonly property int cornerLarge: 16
    readonly property int cornerExtraLarge: 28

    readonly property real hoverOpacity: 0.08
    readonly property real pressedOpacity: 0.10
    readonly property real focusOpacity: 0.10

    readonly property int labelLarge: 14
    readonly property int labelMedium: 12
    readonly property int bodyMedium: 14
    readonly property int titleMedium: 16
    readonly property int titleLarge: 22

    readonly property int enterDuration: 400
    readonly property int exitDuration: 200
    readonly property var emphasizedDecelerate: [0.05, 0.7, 0.1, 1.0, 1.0, 1.0]
    readonly property var emphasizedAccelerate: [0.3, 0.0, 0.8, 0.15, 1.0, 1.0]
}
