// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

Window {
    id: popup

    property Item anchorItem
    property real cardX: 0
    property real cardY: 0
    readonly property bool overlay: Qt.platform.pluginName === "wayland"

    function toggle() {
        if (visible) {
            hide()
            return
        }
        calendar.today()
        const point = anchorItem.mapToGlobal(anchorItem.width / 2, anchorItem.height)
        cardX = Math.round(point.x - card.width / 2)
        cardY = Math.round(point.y + 4)

        if (overlay) {
            shell.show()
            return
        }
        width = card.width
        height = card.height
        x = cardX
        y = cardY
        show()
    }

    flags: Qt.Popup | Qt.FramelessWindowHint
    color: overlay ? "transparent" : Theme.surfaceHigh

    Calendar {
        id: calendar
    }

    PanelShell {
        id: shell

        window: popup
        kind: PanelShell.Overlay
    }

    Shortcut {
        sequence: "Escape"
        onActivated: popup.hide()
    }

    MouseArea {
        anchors.fill: parent
        enabled: popup.overlay
        onClicked: popup.hide()
    }

    Rectangle {
        id: card

        x: popup.overlay ? popup.cardX : 0
        y: popup.overlay ? popup.cardY : 0
        width: 336
        height: content.implicitHeight + 32
        radius: popup.overlay ? 28 : 0
        color: Theme.surfaceHigh
        border.width: 1
        border.color: Theme.outlineVariant

        MouseArea {
            anchors.fill: parent
        }

        Column {
            id: content

            x: 16
            y: 16
            width: parent.width - 32
            spacing: 8

            Item {
                width: parent.width
                height: 40

                Text {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: calendar.title
                    font.pixelSize: 18
                    font.weight: Font.Medium
                    color: Theme.onSurface
                }

                Row {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    IconButton {
                        icon: "arrow_back"
                        label: "Previous month"
                        onClicked: calendar.previous()
                    }

                    IconButton {
                        icon: "arrow_forward"
                        label: "Next month"
                        onClicked: calendar.next()
                    }
                }
            }

            Grid {
                width: parent.width
                columns: 7

                Repeater {
                    model: calendar.weekdays

                    Text {
                        required property string modelData

                        width: parent.width / 7
                        height: 28
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: modelData
                        font.pixelSize: 12
                        color: Theme.onSurfaceVariant
                    }
                }

                Repeater {
                    model: calendar.days

                    Item {
                        required property var modelData

                        width: parent.width / 7
                        height: 40

                        Rectangle {
                            anchors.centerIn: parent
                            width: 36
                            height: 36
                            radius: 18
                            color: modelData.today ? Theme.primary : "transparent"

                            Text {
                                anchors.centerIn: parent
                                text: modelData.day
                                font.pixelSize: 14
                                font.weight: modelData.today ? Font.DemiBold : Font.Normal
                                color: modelData.today ? Theme.onPrimary
                                    : (modelData.inMonth ? Theme.onSurface : Qt.alpha(Theme.onSurfaceVariant, 0.5))
                            }
                        }
                    }
                }
            }
        }
    }
}
