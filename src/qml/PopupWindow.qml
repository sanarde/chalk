// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

Window {
    id: popup

    property Item anchorItem
    property int cardWidth: 360
    property real contentHeight: 100
    property bool alignRight: false
    property real cardX: 0
    property real cardY: 0
    property bool shown: false
    readonly property bool overlay: Qt.platform.pluginName === "wayland"
    default property alias content: card.data

    signal opening

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
        opening()
        const below = anchorItem.mapToGlobal(0, anchorItem.height)
        const edge = anchorItem.mapToGlobal(anchorItem.width, 0)
        const wanted = alignRight ? edge.x - cardWidth : below.x + (anchorItem.width - cardWidth) / 2
        const limit = screen ? screen.width : 1920
        cardX = Math.round(Math.max(8, Math.min(wanted, limit - cardWidth - 8)))
        cardY = Math.round(below.y + 4)

        closeTimer.stop()
        shown = true
        if (overlay) {
            shell.show()
            return
        }
        width = cardWidth
        height = contentHeight
        x = cardX
        y = cardY
        show()
    }

    function round() {
        if (!overlay && visible)
            shell.roundCorners(Tokens.cornerExtraLarge)
    }

    flags: Qt.Popup | Qt.FramelessWindowHint
    color: overlay ? "transparent" : Theme.surfaceHigh
    onVisibleChanged: {
        if (!visible)
            shown = false
        round()
    }
    onContentHeightChanged: {
        if (visible && !overlay)
            height = contentHeight
    }
    onWidthChanged: round()
    onHeightChanged: round()

    Timer {
        id: closeTimer

        interval: Tokens.exitDuration
        onTriggered: popup.hide()
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
        width: popup.cardWidth
        height: popup.contentHeight
        radius: Tokens.cornerExtraLarge
        color: Theme.surfaceHigh
        border.width: 1
        border.color: Theme.outlineVariant
        opacity: popup.shown ? 1 : 0
        scale: popup.shown ? 1 : 0.92
        transformOrigin: Item.Top

        Repeater {
            model: popup.overlay ? 8 : 0

            Rectangle {
                required property int index

                z: -1
                anchors.fill: parent
                anchors.topMargin: -index
                anchors.leftMargin: -index * 1.5
                anchors.rightMargin: -index * 1.5
                anchors.bottomMargin: -index * 2 - 2
                radius: card.radius + index * 1.5
                color: Qt.alpha("black", 0.045)
            }
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
    }
}
