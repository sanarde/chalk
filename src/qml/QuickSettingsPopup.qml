// SPDX-License-Identifier: GPL-3.0-only

import QtQuick
import Sanar.Chalk
import Sanar.Style

PopupWindow {
    id: popup

    required property QuickSettings settings
    required property NotificationStore notifications

    alignRight: true
    contentHeight: content.implicitHeight + 32
    onShownChanged: settings.watching = shown
    onOpening: settings.refresh()

    function volumeIcon() {
        if (settings.muted || settings.volume === 0)
            return "volume_off"
        return settings.volume < 0.5 ? "volume_down" : "volume_up"
    }

    Column {
        id: content

        x: 16
        y: 16
        width: parent.width - 32
        spacing: 12

        Item {
            width: parent.width
            height: 40
            visible: popup.settings.batteryPercent >= 0

            Icon {
                id: batteryIcon

                anchors.verticalCenter: parent.verticalCenter
                name: "battery_full"
                color: Theme.onSurface
                font.pixelSize: 24
            }

            Text {
                anchors.left: batteryIcon.right
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: popup.settings.batteryPercent + "%" + (popup.settings.batteryCharging ? "  " + qsTr("Charging") : "")
                font.pixelSize: Tokens.titleMedium
                color: Theme.onSurface
            }
        }

        Grid {
            width: parent.width
            columns: 2
            spacing: 8

            QuickTile {
                width: (parent.width - 8) / 2
                visible: popup.settings.wifiAvailable
                icon: popup.settings.wifiEnabled ? "wifi" : "wifi_off"
                title: qsTr("Wi-Fi")
                subtitle: !popup.settings.wifiEnabled ? qsTr("Off")
                    : (popup.settings.wifiName || qsTr("Not connected"))
                checked: popup.settings.wifiEnabled
                onClicked: popup.settings.setWifiEnabled(!popup.settings.wifiEnabled)
            }

            QuickTile {
                width: (parent.width - 8) / 2
                icon: popup.notifications.doNotDisturb ? "notifications_off" : "notifications"
                title: qsTr("Do not disturb")
                subtitle: popup.notifications.doNotDisturb ? qsTr("On") : qsTr("Off")
                checked: popup.notifications.doNotDisturb
                onClicked: popup.notifications.doNotDisturb = !popup.notifications.doNotDisturb
            }

            QuickTile {
                width: (parent.width - 8) / 2
                icon: Theme.dark ? "dark_mode" : "light_mode"
                title: qsTr("Dark theme")
                subtitle: Theme.dark ? qsTr("On") : qsTr("Off")
                checked: Theme.dark
                onClicked: Theme.mode = Theme.dark ? Theme.Light : Theme.Dark
            }

            QuickTile {
                width: (parent.width - 8) / 2
                visible: popup.settings.audioAvailable
                icon: popup.settings.muted ? "volume_off" : "volume_up"
                title: qsTr("Sound")
                subtitle: popup.settings.muted ? qsTr("Muted") : Math.round(popup.settings.volume * 100) + "%"
                checked: !popup.settings.muted
                onClicked: popup.settings.toggleMute()
            }
        }

        Slider {
            width: parent.width
            visible: popup.settings.audioAvailable
            icon: popup.volumeIcon()
            label: qsTr("Volume")
            value: popup.settings.volume
            onMoved: value => popup.settings.setVolume(value)
        }

        Slider {
            width: parent.width
            visible: popup.settings.brightnessAvailable
            icon: "brightness_medium"
            label: qsTr("Brightness")
            value: popup.settings.brightness
            onMoved: value => popup.settings.setBrightness(value)
        }
    }
}
