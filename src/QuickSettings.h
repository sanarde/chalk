// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

namespace Chalk {

struct VolumeState {
    bool valid = false;
    double volume = 0;
    bool muted = false;
};

class QuickSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(double volume READ volume NOTIFY changed FINAL)
    Q_PROPERTY(bool muted READ muted NOTIFY changed FINAL)
    Q_PROPERTY(bool audioAvailable READ audioAvailable NOTIFY changed FINAL)
    Q_PROPERTY(double brightness READ brightness NOTIFY changed FINAL)
    Q_PROPERTY(bool brightnessAvailable READ brightnessAvailable NOTIFY changed FINAL)
    Q_PROPERTY(bool wifiAvailable READ wifiAvailable NOTIFY changed FINAL)
    Q_PROPERTY(bool wifiEnabled READ wifiEnabled NOTIFY changed FINAL)
    Q_PROPERTY(QString wifiName READ wifiName NOTIFY changed FINAL)
    Q_PROPERTY(int batteryPercent READ batteryPercent NOTIFY changed FINAL)
    Q_PROPERTY(bool batteryCharging READ batteryCharging NOTIFY changed FINAL)
    Q_PROPERTY(bool watching READ watching WRITE setWatching NOTIFY watchingChanged FINAL)

public:
    explicit QuickSettings(QObject *parent = nullptr);

    double volume() const { return m_volume; }
    bool muted() const { return m_muted; }
    bool audioAvailable() const { return m_audio; }
    double brightness() const { return m_brightness; }
    bool brightnessAvailable() const { return m_brightness >= 0; }
    bool wifiAvailable() const { return m_wifiAvailable; }
    bool wifiEnabled() const { return m_wifiEnabled; }
    QString wifiName() const { return m_wifiName; }
    int batteryPercent() const { return m_battery; }
    bool batteryCharging() const { return m_charging; }
    bool watching() const { return m_watching; }
    void setWatching(bool watching);

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setVolume(double volume);
    Q_INVOKABLE void toggleMute();
    Q_INVOKABLE void setBrightness(double brightness);
    Q_INVOKABLE void setWifiEnabled(bool enabled);

    static VolumeState parseVolume(const QString &text);
    static QString parseActiveNetwork(const QString &text);
    static double clampUnit(double value);

signals:
    void changed();
    void watchingChanged();

private:
    void refreshAudio();
    void refreshWifi();
    void refreshBrightness();
    void refreshBattery();
    void run(const QString &program, const QStringList &arguments, std::function<void(const QString &)> done = {});

    QTimer m_timer;
    bool m_audio = false;
    double m_volume = 0;
    bool m_muted = false;
    double m_brightness = -1;
    QString m_backlight;
    int m_maxBrightness = 0;
    bool m_wifiAvailable = false;
    bool m_wifiEnabled = false;
    QString m_wifiName;
    int m_battery = -1;
    bool m_charging = false;
    bool m_watching = false;
};

}
