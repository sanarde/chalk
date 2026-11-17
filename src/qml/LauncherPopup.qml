// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

PopupWindow {
    id: launcher

    readonly property int rowHeight: 56
    readonly property int visibleRows: 7

    cardWidth: 480
    contentHeight: 56 + 1 + (apps.count > 0 ? Math.min(apps.count, visibleRows) * rowHeight + 16 : 72)

    function activate(row) {
        if (apps.launch(row))
            launcher.dismiss()
    }

    onOpening: {
        apps.reload()
        field.text = ""
        list.currentIndex = 0
    }

    onShownChanged: {
        if (shown)
            field.forceActiveFocus()
    }

    AppCatalog {
        id: apps

        query: field.text

        onCountChanged: list.currentIndex = 0
    }

    Item {
        id: searchBar

        x: 8
        y: 0
        width: parent.width - 16
        height: 56

        Icon {
            id: searchIcon

            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            name: "search"
            font.pixelSize: 24
            color: Theme.onSurfaceVariant
        }

        TextInput {
            id: field

            anchors.left: searchIcon.right
            anchors.leftMargin: 16
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: Tokens.bodyLarge
            color: Theme.onSurface
            selectionColor: Theme.primary
            selectedTextColor: Theme.onPrimary
            clip: true
            focus: true
            Accessible.role: Accessible.EditableText
            Accessible.name: qsTr("Search applications")

            Keys.onDownPressed: list.currentIndex = Math.min(list.currentIndex + 1, apps.count - 1)
            Keys.onUpPressed: list.currentIndex = Math.max(list.currentIndex - 1, 0)
            Keys.onReturnPressed: launcher.activate(list.currentIndex)
            Keys.onEnterPressed: launcher.activate(list.currentIndex)

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                visible: field.text.length === 0
                text: qsTr("Search applications")
                font: field.font
                color: Theme.onSurfaceVariant
            }
        }
    }

    Rectangle {
        id: rule

        anchors.top: searchBar.bottom
        width: parent.width
        height: 1
        color: Theme.outlineVariant
    }

    ListView {
        id: list

        anchors.top: rule.bottom
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        height: Math.min(apps.count, launcher.visibleRows) * launcher.rowHeight
        clip: true
        model: apps
        currentIndex: 0
        highlightMoveDuration: 0
        boundsBehavior: Flickable.StopAtBounds

        delegate: Item {
            id: row

            required property int index
            required property string name
            required property string comment
            required property string icon

            width: list.width
            height: launcher.rowHeight

            Rectangle {
                anchors.fill: parent
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                radius: Tokens.cornerLarge
                color: Theme.secondaryContainer
                opacity: row.index === list.currentIndex ? 1 : 0
            }

            StateLayer {
                anchors.fill: parent
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                radius: Tokens.cornerLarge
                hovered: hover.containsMouse
                pressed: hover.pressed
            }

            Rectangle {
                id: badge

                x: 12
                anchors.verticalCenter: parent.verticalCenter
                width: 40
                height: 40
                radius: 20
                color: Theme.primaryContainer

                Image {
                    id: appImage

                    anchors.centerIn: parent
                    width: 28
                    height: 28
                    sourceSize: Qt.size(56, 56)
                    source: row.icon.length > 0 ? "image://appicon/" + row.icon : ""
                }

                Text {
                    anchors.centerIn: parent
                    visible: appImage.status !== Image.Ready
                    text: row.name.charAt(0).toUpperCase()
                    font.pixelSize: Tokens.titleMedium
                    color: Theme.onPrimaryContainer
                }
            }

            Column {
                anchors.left: badge.right
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    width: parent.width
                    text: row.name
                    elide: Text.ElideRight
                    font.pixelSize: Tokens.bodyLarge
                    color: Theme.onSurface
                }

                Text {
                    width: parent.width
                    visible: row.comment.length > 0
                    text: row.comment
                    elide: Text.ElideRight
                    font.pixelSize: Tokens.bodyMedium
                    color: Theme.onSurfaceVariant
                }
            }

            MouseArea {
                id: hover

                anchors.fill: parent
                hoverEnabled: true
                onEntered: list.currentIndex = row.index
                onClicked: launcher.activate(row.index)
            }
        }
    }

    Text {
        anchors.top: rule.bottom
        anchors.topMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        visible: apps.count === 0
        text: qsTr("No applications found")
        font.pixelSize: Tokens.bodyLarge
        color: Theme.onSurfaceVariant
    }
}
