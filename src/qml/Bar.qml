// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

Window {
    id: bar

    property bool openCalendar: false
    property bool openLauncher: false

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
        thickness: 48
    }

    NotificationStore {
        id: notifications
    }

    NotificationServer {
        id: notificationServer

        store: notifications
        Component.onCompleted: start()
    }

    NotificationBanner {
        store: notifications
    }

    QuickSettings {
        id: quick
    }

    QuickSettingsPopup {
        id: quickPopup

        anchorItem: statusPill
        transientParent: bar
        settings: quick
        notifications: notifications
    }

    LauncherPopup {
        id: launcherPopup

        anchorItem: launcherButton
        transientParent: bar
    }

    CalendarPopup {
        id: calendarPopup

        notifications: notifications
        anchorItem: clockButton
        transientParent: bar
    }

    Component.onCompleted: shell.show()

    onWidthChanged: {
        if (openCalendar && width > 100) {
            openCalendar = false
            Qt.callLater(() => calendarPopup.toggle())
        }
        if (openLauncher && width > 100) {
            openLauncher = false
            Qt.callLater(() => launcherPopup.toggle())
        }
    }

    Row {
        id: leftRow

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        IconButton {
            anchors.verticalCenter: parent.verticalCenter
            icon: "grid_view"
            iconColor: Theme.onSurface
            label: qsTr("Activities")
            onClicked: slate.action("overview")
        }

        IconButton {
            id: launcherButton

            anchors.verticalCenter: parent.verticalCenter
            icon: "search"
            iconColor: Theme.onSurface
            label: qsTr("Search applications")
            onClicked: launcherPopup.toggle()
        }
    }

    Row {
        id: windowList

        readonly property real room: clockButton.x - x - 16
        readonly property int count: Math.max(1, slate.windows.length)

        anchors.left: leftRow.right
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4

        Repeater {
            model: slate.windows

            WindowChip {
                required property var modelData

                width: Math.max(40, Math.min(168, (windowList.room - (windowList.count - 1) * 4) / windowList.count))
                title: modelData.title
                app: modelData.app
                focused: modelData.focused
                minimized: modelData.minimized
                onClicked: slate.activateWindow(modelData.id)
            }
        }
    }

    PillButton {
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

        StatusPill {
            id: statusPill

            anchors.verticalCenter: parent.verticalCenter
            settings: quick
            quiet: notifications.doNotDisturb
            highlighted: quickPopup.visible
            onClicked: quickPopup.toggle()
        }
    }
}
