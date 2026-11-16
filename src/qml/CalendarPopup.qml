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
    property string selector: ""
    readonly property var yearList: Array.from({ length: 201 }, (_, offset) => 1926 + offset)
    property int selectedYear: 0
    property int selectedMonth: 0
    property int selectedDay: 0
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
        selector = ""
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
        width: 360
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

                Row {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 4

                    IconButton {
                        implicitWidth: 32
                        implicitHeight: 32
                        iconSize: 20
                        icon: "chevron_right"
                        rotation: 180
                        label: qsTr("Previous month")
                        onClicked: calendar.previous()
                    }

                    DropdownButton {
                        text: calendar.monthName
                        expanded: popup.selector === "month"
                        onClicked: popup.selector = popup.selector === "month" ? "" : "month"
                    }

                    IconButton {
                        implicitWidth: 32
                        implicitHeight: 32
                        iconSize: 20
                        icon: "chevron_right"
                        label: qsTr("Next month")
                        onClicked: calendar.next()
                    }
                }

                Row {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 4

                    IconButton {
                        implicitWidth: 32
                        implicitHeight: 32
                        iconSize: 20
                        icon: "chevron_right"
                        rotation: 180
                        label: qsTr("Previous year")
                        onClicked: calendar.show(calendar.year - 1, calendar.month)
                    }

                    DropdownButton {
                        text: calendar.year
                        expanded: popup.selector === "year"
                        onClicked: popup.selector = popup.selector === "year" ? "" : "year"
                    }

                    IconButton {
                        implicitWidth: 32
                        implicitHeight: 32
                        iconSize: 20
                        icon: "chevron_right"
                        label: qsTr("Next year")
                        onClicked: calendar.show(calendar.year + 1, calendar.month)
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
                        height: 40
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: modelData
                        font.pixelSize: Tokens.bodyLarge
                        color: Theme.onSurface
                    }
                }

                Repeater {
                    model: calendar.days

                    Item {
                        id: cell

                        required property var modelData
                        readonly property bool selected: modelData.inMonth && popup.selectedYear === calendar.year
                            && popup.selectedMonth === calendar.month && popup.selectedDay === modelData.day

                        width: parent.width / 7
                        height: 40

                        Rectangle {
                            anchors.centerIn: parent
                            width: 40
                            height: 40
                            radius: 20
                            color: cell.selected ? Theme.primary : "transparent"
                            border.width: cell.modelData.today && !cell.selected ? 1 : 0
                            border.color: Theme.primary

                            StateLayer {
                                anchors.fill: parent
                                radius: parent.radius
                                color: cell.selected ? Theme.onPrimary : Theme.onSurface
                                hovered: cellHover.hovered
                                pressed: cellTap.pressed
                            }

                            Text {
                                anchors.centerIn: parent
                                text: cell.modelData.day
                                font.pixelSize: Tokens.bodyLarge
                                color: cell.selected ? Theme.onPrimary
                                    : (cell.modelData.today ? Theme.primary
                                    : (cell.modelData.inMonth ? Theme.onSurface : Qt.alpha(Theme.onSurfaceVariant, 0.5)))
                            }

                            HoverHandler {
                                id: cellHover
                            }

                            TapHandler {
                                id: cellTap

                                enabled: cell.modelData.inMonth
                                onTapped: {
                                    popup.selectedYear = calendar.year
                                    popup.selectedMonth = calendar.month
                                    popup.selectedDay = cell.modelData.day
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: chooser

            x: content.x
            y: content.y + 48
            width: content.width
            height: content.height - 48
            visible: popup.selector !== ""
            radius: Tokens.cornerMedium
            color: Theme.surfaceHigh
            clip: true

            ListView {
                id: choices

                anchors.fill: parent
                model: popup.selector === "month" ? calendar.months : yearList
                currentIndex: popup.selector === "month" ? calendar.month - 1 : calendar.year - yearList[0]
                onVisibleChanged: if (visible) positionViewAtIndex(Math.max(0, currentIndex - 2), ListView.Beginning)

                delegate: Item {
                    id: choice

                    required property int index
                    required property var modelData
                    readonly property bool current: index === choices.currentIndex

                    width: choices.width
                    height: 48

                    Rectangle {
                        anchors.fill: parent
                        color: choice.current ? Theme.secondaryContainer : "transparent"
                    }

                    StateLayer {
                        anchors.fill: parent
                        hovered: choiceHover.hovered
                        pressed: choiceTap.pressed
                    }

                    Icon {
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.verticalCenter: parent.verticalCenter
                        name: "check"
                        visible: choice.current
                        color: Theme.onSecondaryContainer
                        font.pixelSize: 20
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 48
                        anchors.verticalCenter: parent.verticalCenter
                        text: choice.modelData
                        font.pixelSize: Tokens.bodyLarge
                        color: choice.current ? Theme.onSecondaryContainer : Theme.onSurface
                    }

                    HoverHandler {
                        id: choiceHover
                    }

                    TapHandler {
                        id: choiceTap

                        onTapped: {
                            if (popup.selector === "month")
                                calendar.show(calendar.year, choice.index + 1)
                            else
                                calendar.show(choice.modelData, calendar.month)
                            popup.selector = ""
                        }
                    }
                }
            }
        }
    }
}
