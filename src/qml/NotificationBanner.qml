// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

Window {
    id: banner

    required property NotificationStore store

    readonly property var current: store.banner
    readonly property bool wanted: current.id !== undefined
    readonly property bool critical: current.urgency === 2
    readonly property bool onWayland: Qt.platform.pluginName === "wayland"
    property var shownData: ({})
    property bool shown: false

    width: 400
    height: 112
    color: onWayland ? "transparent" : Theme.surfaceHigh
    title: "Chalk notification"
    visible: false

    onWantedChanged: {
        if (wanted) {
            shownData = current
            closeTimer.stop()
            shown = true
            shell.show()
        } else {
            shown = false
            closeTimer.restart()
        }
    }

    onCurrentChanged: {
        if (wanted)
            shownData = current
    }

    onVisibleChanged: if (visible && !onWayland) shell.roundCorners(Tokens.cornerLarge)

    PanelShell {
        id: shell

        window: banner
        kind: PanelShell.Banner
        margin: 56
    }

    Timer {
        id: closeTimer

        interval: Tokens.exitDuration
        onTriggered: banner.hide()
    }

    Rectangle {
        id: card

        anchors.fill: parent
        anchors.margins: banner.onWayland ? 8 : 0
        radius: Tokens.cornerLarge
        color: Theme.surfaceHigh
        border.width: 1
        border.color: banner.critical ? Theme.onErrorContainer : Theme.outlineVariant
        opacity: banner.shown ? 1 : 0
        transform: Translate {
            y: banner.shown ? 0 : -12

            Behavior on y {
                NumberAnimation {
                    duration: banner.shown ? Tokens.enterDuration : Tokens.exitDuration
                    easing.type: Easing.BezierSpline
                    easing.bezierCurve: banner.shown ? Tokens.emphasizedDecelerate : Tokens.emphasizedAccelerate
                }
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: banner.shown ? Tokens.enterDuration : Tokens.exitDuration
                easing.type: Easing.BezierSpline
                easing.bezierCurve: banner.shown ? Tokens.emphasizedDecelerate : Tokens.emphasizedAccelerate
            }
        }

        Repeater {
            model: banner.onWayland ? 6 : 0

            Rectangle {
                required property int index

                z: -1
                anchors.fill: parent
                anchors.margins: -index
                anchors.topMargin: -index + 1
                anchors.bottomMargin: -index - 1
                radius: card.radius + index
                color: Qt.alpha("black", 0.05)
            }
        }

        TapHandler {
            onTapped: {
                const actions = banner.shownData.actions || []
                if (actions.indexOf("default") >= 0)
                    banner.store.invoke(banner.shownData.id, "default")
                else
                    banner.store.hideBanner()
            }
        }

        Rectangle {
            id: badge

            x: 16
            y: 16
            width: 40
            height: 40
            radius: 20
            color: banner.critical ? Theme.errorContainer : Theme.primaryContainer

            Icon {
                anchors.centerIn: parent
                name: banner.critical ? "error" : "info"
                color: banner.critical ? Theme.onErrorContainer : Theme.onPrimaryContainer
                font.pixelSize: 24
            }
        }

        Column {
            anchors.left: badge.right
            anchors.leftMargin: 16
            anchors.right: closeButton.left
            anchors.rightMargin: 4
            anchors.top: parent.top
            anchors.topMargin: 14
            spacing: 2

            Text {
                width: parent.width
                elide: Text.ElideRight
                text: banner.shownData.app || ""
                font.pixelSize: Tokens.labelMedium
                color: Theme.onSurfaceVariant
            }

            Text {
                width: parent.width
                elide: Text.ElideRight
                maximumLineCount: 1
                text: banner.shownData.summary || ""
                font.pixelSize: Tokens.titleMedium
                color: Theme.onSurface
            }

            Text {
                width: parent.width
                elide: Text.ElideRight
                maximumLineCount: actionRow.visible ? 1 : 2
                wrapMode: Text.WordWrap
                textFormat: Text.StyledText
                text: banner.shownData.body || ""
                font.pixelSize: Tokens.bodyMedium
                color: Theme.onSurfaceVariant
            }
        }

        IconButton {
            id: closeButton

            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 8
            implicitWidth: 32
            implicitHeight: 32
            iconSize: 20
            icon: "close"
            label: qsTr("Dismiss")
            onClicked: banner.store.hideBanner()
        }

        Row {
            id: actionRow

            readonly property var named: {
                const list = banner.shownData.actions || []
                const result = []
                for (let index = 0; index + 1 < list.length; index += 2) {
                    if (list[index] !== "default")
                        result.push({ key: list[index], label: list[index + 1] })
                }
                return result
            }

            visible: named.length > 0
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            spacing: 4

            Repeater {
                model: actionRow.named

                PillButton {
                    required property var modelData

                    text: modelData.label
                    textColor: Theme.primary
                    onClicked: banner.store.invoke(banner.shownData.id, modelData.key)
                }
            }
        }
    }
}
