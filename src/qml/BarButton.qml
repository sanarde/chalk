// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Style

Rectangle {
    id: button

    property alias text: label.text
    property alias font: label.font
    property color textColor: Theme.onSurface
    property bool highlighted: false

    signal clicked

    implicitWidth: label.implicitWidth + 24
    implicitHeight: 28
    radius: height / 2
    color: highlighted ? Theme.secondaryContainer : (hover.hovered ? Theme.surfaceHigh : "transparent")

    Text {
        id: label

        anchors.centerIn: parent
        font.pixelSize: 14
        font.weight: Font.Medium
        color: button.highlighted ? Theme.onSecondaryContainer : button.textColor
    }

    HoverHandler {
        id: hover
    }

    TapHandler {
        onTapped: button.clicked()
    }
}
