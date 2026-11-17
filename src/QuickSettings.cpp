// SPDX-License-Identifier: GPL-3.0-only

#include "QuickSettings.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QtGlobal>

#include <algorithm>
#include <functional>

namespace Chalk {

namespace {

constexpr int IdleInterval = 10000;
constexpr int WatchingInterval = 1500;

QString readText(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};
    return QString::fromUtf8(file.read(256)).trimmed();
}

}

QuickSettings::QuickSettings(QObject *parent)
    : QObject(parent)
{
    m_timer.setInterval(IdleInterval);
    connect(&m_timer, &QTimer::timeout, this, &QuickSettings::refresh);
    m_timer.start();
    QTimer::singleShot(0, this, &QuickSettings::refresh);
}

double QuickSettings::clampUnit(double value)
{
    return std::clamp(value, 0.0, 1.0);
}

VolumeState QuickSettings::parseVolume(const QString &text)
{
    VolumeState state;
    const QString line = text.trimmed();
    if (!line.startsWith(QLatin1String("Volume:")))
        return state;

    const QStringList words = line.mid(7).simplified().split(QLatin1Char(' '));
    if (words.isEmpty())
        return state;
    bool ok = false;
    const double value = words.first().toDouble(&ok);
    if (!ok || value < 0)
        return state;

    state.valid = true;
    state.volume = clampUnit(value);
    state.muted = line.contains(QLatin1String("[MUTED]"));
    return state;
}

QString QuickSettings::parseActiveNetwork(const QString &text)
{
    const QStringList lines = text.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (!line.startsWith(QLatin1String("yes:")))
            continue;
        QString name;
        for (qsizetype index = 4; index < line.size(); ++index) {
            if (line.at(index) == QLatin1Char('\\')) {
                if (index + 1 >= line.size())
                    break;
                ++index;
            }
            name.append(line.at(index));
        }
        return name;
    }
    return {};
}

void QuickSettings::setWatching(bool watching)
{
    if (m_watching == watching)
        return;
    m_watching = watching;
    m_timer.setInterval(watching ? WatchingInterval : IdleInterval);
    if (watching)
        refresh();
    emit watchingChanged();
}

void QuickSettings::run(const QString &program, const QStringList &arguments, std::function<void(const QString &)> done)
{
    auto *process = new QProcess(this);
    connect(process, &QProcess::finished, this, [process, done = std::move(done)](int code, QProcess::ExitStatus) {
        if (done && code == 0)
            done(QString::fromUtf8(process->readAllStandardOutput()));
        process->deleteLater();
    });
    connect(process, &QProcess::errorOccurred, process, [process](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart)
            process->deleteLater();
    });
    process->start(program, arguments);
}

void QuickSettings::refresh()
{
    refreshAudio();
    refreshWifi();
    refreshBrightness();
    refreshBattery();
}

void QuickSettings::refreshAudio()
{
    run(QStringLiteral("wpctl"), {QStringLiteral("get-volume"), QStringLiteral("@DEFAULT_AUDIO_SINK@")},
        [this](const QString &output) {
            const VolumeState state = parseVolume(output);
            if (state.valid == m_audio && qFuzzyCompare(state.volume + 1, m_volume + 1) && state.muted == m_muted)
                return;
            m_audio = state.valid;
            m_volume = state.volume;
            m_muted = state.muted;
            emit changed();
        });
}

void QuickSettings::refreshWifi()
{
    run(QStringLiteral("nmcli"), {QStringLiteral("-t"), QStringLiteral("-f"), QStringLiteral("WIFI"), QStringLiteral("g")},
        [this](const QString &output) {
            const QString state = output.trimmed();
            const bool enabled = state == QLatin1String("enabled");
            const bool available = enabled || state == QLatin1String("disabled");
            if (!enabled) {
                if (m_wifiAvailable != available || m_wifiEnabled || !m_wifiName.isEmpty()) {
                    m_wifiAvailable = available;
                    m_wifiEnabled = false;
                    m_wifiName.clear();
                    emit changed();
                }
                return;
            }
            run(QStringLiteral("nmcli"),
                {QStringLiteral("-t"), QStringLiteral("-f"), QStringLiteral("ACTIVE,SSID"), QStringLiteral("dev"),
                 QStringLiteral("wifi")},
                [this](const QString &list) {
                    const QString name = parseActiveNetwork(list);
                    if (m_wifiAvailable && m_wifiEnabled && m_wifiName == name)
                        return;
                    m_wifiAvailable = true;
                    m_wifiEnabled = true;
                    m_wifiName = name;
                    emit changed();
                });
        });
}

void QuickSettings::refreshBrightness()
{
    if (m_backlight.isEmpty()) {
        const QStringList names = QDir(QStringLiteral("/sys/class/backlight")).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        if (names.isEmpty())
            return;
        m_backlight = names.first();
        m_maxBrightness = readText(QStringLiteral("/sys/class/backlight/%1/max_brightness").arg(m_backlight)).toInt();
    }
    if (m_maxBrightness <= 0)
        return;
    const int current = readText(QStringLiteral("/sys/class/backlight/%1/brightness").arg(m_backlight)).toInt();
    const double value = clampUnit(static_cast<double>(current) / m_maxBrightness);
    if (qFuzzyCompare(value + 1, m_brightness + 1))
        return;
    m_brightness = value;
    emit changed();
}

void QuickSettings::refreshBattery()
{
    const QStringList names = QDir(QStringLiteral("/sys/class/power_supply")).entryList({QStringLiteral("BAT*")}, QDir::Dirs);
    int percent = -1;
    bool charging = false;
    if (!names.isEmpty()) {
        const QString base = QStringLiteral("/sys/class/power_supply/") + names.first();
        bool ok = false;
        const int value = readText(base + QStringLiteral("/capacity")).toInt(&ok);
        if (ok)
            percent = std::clamp(value, 0, 100);
        charging = readText(base + QStringLiteral("/status")) == QLatin1String("Charging");
    }
    if (percent == m_battery && charging == m_charging)
        return;
    m_battery = percent;
    m_charging = charging;
    emit changed();
}

void QuickSettings::setVolume(double volume)
{
    const double value = clampUnit(volume);
    m_volume = value;
    emit changed();
    run(QStringLiteral("wpctl"),
        {QStringLiteral("set-volume"), QStringLiteral("@DEFAULT_AUDIO_SINK@"), QString::number(value, 'f', 2)});
}

void QuickSettings::toggleMute()
{
    m_muted = !m_muted;
    emit changed();
    run(QStringLiteral("wpctl"), {QStringLiteral("set-mute"), QStringLiteral("@DEFAULT_AUDIO_SINK@"), QStringLiteral("toggle")});
}

void QuickSettings::setBrightness(double brightness)
{
    if (m_backlight.isEmpty() || m_maxBrightness <= 0)
        return;
    const double value = clampUnit(brightness);
    const uint raw = static_cast<uint>(std::max(1, qRound(value * m_maxBrightness)));
    m_brightness = static_cast<double>(raw) / m_maxBrightness;
    emit changed();

    QDBusMessage message = QDBusMessage::createMethodCall(
        QStringLiteral("org.freedesktop.login1"), QStringLiteral("/org/freedesktop/login1/session/auto"),
        QStringLiteral("org.freedesktop.login1.Session"), QStringLiteral("SetBrightness"));
    message << QStringLiteral("backlight") << m_backlight << raw;
    QDBusConnection::systemBus().asyncCall(message);
}

void QuickSettings::setWifiEnabled(bool enabled)
{
    m_wifiEnabled = enabled;
    if (!enabled)
        m_wifiName.clear();
    emit changed();
    run(QStringLiteral("nmcli"), {QStringLiteral("radio"), QStringLiteral("wifi"),
                                  enabled ? QStringLiteral("on") : QStringLiteral("off")},
        [this](const QString &) { QTimer::singleShot(1500, this, &QuickSettings::refreshWifi); });
}

}
