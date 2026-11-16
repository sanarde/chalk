// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import QtQuick.Effects
import Sanar.Chalk
import Sanar.Style

Window {
    id: popup

    property Item anchorItem
    property real cardX: 0
    property real cardY: 0
    property bool shown: false
    readonly property bool overlay: Qt.platform.pluginName === "wayland"

    function dismiss() {
        if (!shown)
            return
        shown = false
        closeTimer.restart()
    }

    function toggle() {
        if (visible && shown) {
            dismiss()
            return
        }
        calendar.today()
        const point = anchorItem.mapToGlobal(anchorItem.width / 2, anchorItem.height)
        cardX = Math.round(point.x - card.width / 2)
        cardY = Math.round(point.y + 4)

        closeTimer.stop()
        shown = true
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

    Timer {
        id: closeTimer

        interval: Tokens.exitDuration
        onTriggered: popup.hide()
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
        onActivated: popup.dismiss()
    }

    MouseArea {
        anchors.fill: parent
        enabled: popup.overlay
        onClicked: popup.dismiss()
    }

    Rectangle {
        id: card

        x: popup.overlay ? popup.cardX : 0
        y: popup.overlay ? popup.cardY : 0
        width: 336
        height: content.implicitHeight + 32
        radius: popup.overlay ? Tokens.cornerExtraLarge : 0
        color: Theme.surfaceHigh
        border.width: 1
        border.color: Theme.outlineVariant
        opacity: popup.shown ? 1 : 0
        scale: popup.shown ? 1 : 0.92
        transformOrigin: Item.Top
        layer.enabled: popup.overlay
        layer.effect: MultiEffect {
            autoPaddingEnabled: true
            shadowEnabled: true
            shadowColor: Qt.alpha("black", 0.3)
            shadowBlur: 0.8
            shadowVerticalOffset: 6
        }

        Behavior on opacity {
            NumberAnimation {
                duration: popup.shown ? Tokens.enterDuration : Tokens.exitDuration
                easing.type: Easing.BezierSpline
                easing.bezierCurve: popup.shown ? Tokens.emphasizedDecelerate : Tokens.emphasizedAccelerate
            }
        }

        Behavior on scale {
            NumberAnimation {
                duration: popup.shown ? Tokens.enterDuration : Tokens.exitDuration
                easing.type: Easing.BezierSpline
                easing.bezierCurve: popup.shown ? Tokens.emphasizedDecelerate : Tokens.emphasizedAccelerate
            }
        }

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
                    font.pixelSize: Tokens.titleMedium
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
                        font.pixelSize: Tokens.labelMedium
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
                                font.pixelSize: Tokens.bodyMedium
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
