// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Style

Rectangle {
    id: button

    property alias text: label.text
    property color textColor: Theme.onSurface
    property bool highlighted: false

    signal clicked

    implicitWidth: label.implicitWidth + 24
    implicitHeight: 32
    radius: height / 2
    color: highlighted ? Theme.secondaryContainer : "transparent"
    activeFocusOnTab: true

    Accessible.role: Accessible.Button
    Accessible.name: label.text
    Accessible.onPressAction: button.clicked()

    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: Theme.onSurface
        opacity: tap.pressed ? Tokens.pressedOpacity
            : (button.activeFocus ? Tokens.focusOpacity : (hover.hovered ? Tokens.hoverOpacity : 0))
    }

    Text {
        id: label

        anchors.centerIn: parent
        font.pixelSize: Tokens.labelLarge
        font.weight: Font.Medium
        color: button.highlighted ? Theme.onSecondaryContainer : button.textColor
    }

    HoverHandler {
        id: hover
    }

    TapHandler {
        id: tap

        onTapped: button.clicked()
    }

    Keys.onReturnPressed: button.clicked()
    Keys.onSpacePressed: button.clicked()
}
