// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

Window {
    id: bar

    title: "Chalk"
    color: Theme.surface
    visible: false

    SlateClient {
        id: slate
    }

    PanelShell {
        id: shell

        window: bar
        edge: PanelShell.Top
        thickness: 40
    }

    Component.onCompleted: shell.show()

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        Repeater {
            model: slate.workspaces

            WorkspaceChip {
                required property var modelData

                visible: modelData.active || modelData.windows > 0
                index: modelData.index
                windows: modelData.windows
                active: modelData.active
                onClicked: slate.switchWorkspace(modelData.index)
            }
        }
    }

    Text {
        anchors.centerIn: parent
        width: Math.min(implicitWidth, parent.width / 3)
        elide: Text.ElideRight
        horizontalAlignment: Text.AlignHCenter
        text: slate.connected ? slate.focusedTitle : "Slate is not running"
        font.pixelSize: 14
        color: Theme.onSurfaceVariant
    }

    Text {
        id: clock

        property date now: new Date()

        anchors.right: parent.right
        anchors.rightMargin: 16
        anchors.verticalCenter: parent.verticalCenter
        text: Qt.formatDateTime(now, "ddd d MMM  HH:mm")
        font.pixelSize: 14
        font.weight: Font.Medium
        color: Theme.onSurface

        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: clock.now = new Date()
        }
    }
}
