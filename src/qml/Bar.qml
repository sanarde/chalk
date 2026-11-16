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

    CalendarPopup {
        id: calendarPopup

        anchorItem: clockButton
        transientParent: bar
    }

    Component.onCompleted: shell.show()

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        BarButton {
            text: "Activities"
            onClicked: slate.action("overview")
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            leftPadding: 8
            width: Math.min(implicitWidth, bar.width / 4)
            elide: Text.ElideRight
            text: slate.connected ? slate.focusedApp : ""
            font.pixelSize: 14
            font.weight: Font.Medium
            color: Theme.onSurfaceVariant
        }
    }

    BarButton {
        id: clockButton

        property date now: new Date()

        anchors.centerIn: parent
        text: Qt.locale().toString(now, "ddd d MMM  HH:mm")
        highlighted: calendarPopup.visible
        onClicked: calendarPopup.toggle()

        Timer {
            interval: 1000
            running: true
            repeat: true
            onTriggered: clockButton.now = new Date()
        }
    }

    Row {
        anchors.right: parent.right
        anchors.rightMargin: 8
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
}
