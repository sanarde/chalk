// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

PopupWindow {
    id: popup

    property string selector: ""
    property NotificationStore notifications
    readonly property var yearList: Array.from({ length: 201 }, (_, offset) => 1926 + offset)
    property int selectedYear: 0
    property int selectedMonth: 0
    property int selectedDay: 0

    contentHeight: content.implicitHeight + 32
    onOpening: {
        calendar.today()
        selector = ""
    }

    Calendar {
        id: calendar
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
                    icon: "chevron_left"
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
                    icon: "chevron_left"
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

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.outlineVariant
        }

        Item {
            width: parent.width
            height: 40

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Notifications")
                font.pixelSize: Tokens.titleMedium
                color: Theme.onSurface
            }

            PillButton {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: popup.notifications && popup.notifications.count > 0
                text: qsTr("Clear all")
                textColor: Theme.primary
                onClicked: popup.notifications.clear()
            }
        }

        Item {
            width: parent.width
            height: 192

            Text {
                anchors.centerIn: parent
                visible: !popup.notifications || popup.notifications.count === 0
                text: qsTr("No notifications")
                font.pixelSize: Tokens.bodyLarge
                color: Theme.onSurfaceVariant
            }

            ListView {
                anchors.fill: parent
                clip: true
                model: popup.notifications

                delegate: Item {
                    id: entry

                    required property var model

                    width: ListView.view.width
                    height: 64

                    StateLayer {
                        anchors.fill: parent
                        radius: Tokens.cornerMedium
                        hovered: entryHover.hovered
                    }

                    HoverHandler {
                        id: entryHover
                    }

                    Column {
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        anchors.right: entryTime.left
                        anchors.rightMargin: 8
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            width: parent.width
                            elide: Text.ElideRight
                            text: entry.model.summary
                            font.pixelSize: Tokens.bodyLarge
                            color: Theme.onSurface
                        }

                        Text {
                            width: parent.width
                            elide: Text.ElideRight
                            textFormat: Text.StyledText
                            text: entry.model.app + (entry.model.body ? "  " + entry.model.body : "")
                            font.pixelSize: Tokens.labelLarge
                            color: Theme.onSurfaceVariant
                        }
                    }

                    Text {
                        id: entryTime

                        anchors.right: removeButton.left
                        anchors.verticalCenter: parent.verticalCenter
                        text: entry.model.time
                        font.pixelSize: Tokens.labelMedium
                        color: Theme.onSurfaceVariant
                    }

                    IconButton {
                        id: removeButton

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        implicitWidth: 32
                        implicitHeight: 32
                        iconSize: 20
                        icon: "close"
                        label: qsTr("Remove")
                        onClicked: popup.notifications.remove(entry.model.id)
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
        height: 280
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
