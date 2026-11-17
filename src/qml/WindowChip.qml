// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Style

Rectangle {
    id: chip

    required property string title
    required property string app
    required property bool focused
    required property bool minimized

    signal clicked

    implicitWidth: 168
    height: 32
    radius: Tokens.cornerSmall
    color: focused ? Theme.secondaryContainer : "transparent"
    border.width: focused ? 0 : 1
    border.color: Theme.outlineVariant
    opacity: minimized ? 0.6 : 1
    activeFocusOnTab: true

    Accessible.role: Accessible.Button
    Accessible.name: title.length > 0 ? title : app
    Accessible.checked: focused
    Accessible.onPressAction: chip.clicked()

    StateLayer {
        anchors.fill: parent
        radius: parent.radius
        color: chip.focused ? Theme.onSecondaryContainer : Theme.onSurface
        hovered: hover.hovered
        pressed: tap.pressed
        focused: chip.activeFocus
    }

    Rectangle {
        id: badge

        x: 8
        anchors.verticalCenter: parent.verticalCenter
        width: 20
        height: 20
        radius: 10
        color: Theme.primaryContainer

        Text {
            anchors.centerIn: parent
            text: chip.app.charAt(0).toUpperCase()
            font.pixelSize: Tokens.labelMedium
            color: Theme.onPrimaryContainer
        }
    }

    Text {
        anchors.left: badge.right
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        elide: Text.ElideRight
        text: chip.title.length > 0 ? chip.title : chip.app
        font.pixelSize: Tokens.labelLarge
        color: chip.focused ? Theme.onSecondaryContainer : Theme.onSurface
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
