// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Style

Rectangle {
    id: pill

    required property var settings
    required property bool quiet
    property bool highlighted: false

    signal clicked

    implicitWidth: row.implicitWidth + 24
    implicitHeight: 32
    radius: height / 2
    color: highlighted ? Theme.secondaryContainer : "transparent"
    activeFocusOnTab: true

    Accessible.role: Accessible.Button
    Accessible.name: qsTr("Quick settings")
    Accessible.onPressAction: pill.clicked()

    StateLayer {
        anchors.fill: parent
        radius: parent.radius
        hovered: hover.hovered
        pressed: tap.pressed
        focused: pill.activeFocus
    }

    Row {
        id: row

        anchors.centerIn: parent
        spacing: 8

        Icon {
            visible: pill.settings.wifiAvailable
            name: pill.settings.wifiEnabled ? "wifi" : "wifi_off"
            color: Theme.onSurface
            font.pixelSize: 20
        }

        Icon {
            visible: pill.settings.audioAvailable
            name: pill.settings.muted || pill.settings.volume === 0 ? "volume_off"
                : (pill.settings.volume < 0.5 ? "volume_down" : "volume_up")
            color: Theme.onSurface
            font.pixelSize: 20
        }

        Icon {
            visible: pill.quiet
            name: "notifications_off"
            color: Theme.onSurface
            font.pixelSize: 20
        }

        Row {
            visible: pill.settings.batteryPercent >= 0
            spacing: 2
            anchors.verticalCenter: parent.verticalCenter

            Icon {
                name: "battery_full"
                color: Theme.onSurface
                font.pixelSize: 20
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: pill.settings.batteryPercent + "%"
                font.pixelSize: Tokens.labelLarge
                color: Theme.onSurface
            }
        }
    }

    HoverHandler {
        id: hover

        cursorShape: Qt.PointingHandCursor
    }

    TapHandler {
        id: tap

        onTapped: pill.clicked()
    }

    Keys.onReturnPressed: pill.clicked()
    Keys.onSpacePressed: pill.clicked()
}
