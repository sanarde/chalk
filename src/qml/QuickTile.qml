// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Style

Rectangle {
    id: tile

    property string icon
    property string title
    property string subtitle
    property bool checked: false

    signal clicked

    implicitWidth: 156
    implicitHeight: 64
    radius: height / 2
    color: checked ? Theme.primaryContainer : Theme.surfaceHighest
    activeFocusOnTab: true

    Accessible.role: Accessible.CheckBox
    Accessible.name: title
    Accessible.checked: checked
    Accessible.onToggleAction: tile.clicked()
    Accessible.onPressAction: tile.clicked()

    StateLayer {
        anchors.fill: parent
        radius: parent.radius
        color: tile.checked ? Theme.onPrimaryContainer : Theme.onSurface
        hovered: hover.hovered
        pressed: tap.pressed
        focused: tile.activeFocus
    }

    Icon {
        id: glyph

        x: 16
        anchors.verticalCenter: parent.verticalCenter
        name: tile.icon
        filled: tile.checked
        color: tile.checked ? Theme.onPrimaryContainer : Theme.onSurfaceVariant
        font.pixelSize: 24
    }

    Column {
        anchors.left: glyph.right
        anchors.leftMargin: 12
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.verticalCenter: parent.verticalCenter

        Text {
            width: parent.width
            elide: Text.ElideRight
            text: tile.title
            font.pixelSize: Tokens.labelLarge
            font.weight: Font.Medium
            color: tile.checked ? Theme.onPrimaryContainer : Theme.onSurface
        }

        Text {
            width: parent.width
            elide: Text.ElideRight
            text: tile.subtitle
            font.pixelSize: Tokens.labelMedium
            color: tile.checked ? Theme.onPrimaryContainer : Theme.onSurfaceVariant
        }
    }

    HoverHandler {
        id: hover

        cursorShape: Qt.PointingHandCursor
    }

    TapHandler {
        id: tap

        onTapped: tile.clicked()
    }

    Keys.onReturnPressed: tile.clicked()
    Keys.onSpacePressed: tile.clicked()
}
