// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Style

Rectangle {
    id: chip

    required property int index
    required property int windows
    required property bool active

    signal clicked

    width: active ? 56 : 32
    height: 32
    radius: height / 2

    Behavior on width {
        NumberAnimation {
            duration: Tokens.exitDuration
            easing.type: Easing.BezierSpline
            easing.bezierCurve: Tokens.emphasizedDecelerate
        }
    }
    color: active ? Theme.primaryContainer : "transparent"
    activeFocusOnTab: true

    Accessible.role: Accessible.Button
    Accessible.name: qsTr("Workspace %1").arg(index)
    Accessible.checked: active
    Accessible.onPressAction: chip.clicked()

    StateLayer {
        anchors.fill: parent
        radius: parent.radius
        color: chip.active ? Theme.onPrimaryContainer : Theme.onSurface
        hovered: hover.hovered
        pressed: tap.pressed
        focused: chip.activeFocus
    }

    Text {
        anchors.centerIn: parent
        text: chip.index
        font.pixelSize: Tokens.labelLarge
        font.weight: chip.active ? Font.DemiBold : Font.Normal
        color: chip.active ? Theme.onPrimaryContainer : Theme.onSurfaceVariant
    }

    Rectangle {
        visible: !chip.active && chip.windows > 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4
        width: 4
        height: 4
        radius: 2
        color: Theme.primary
    }

    HoverHandler {
        id: hover
    }

    TapHandler {
        id: tap

        onTapped: chip.clicked()
    }

    Keys.onReturnPressed: chip.clicked()
    Keys.onSpacePressed: chip.clicked()
}
