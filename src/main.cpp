// SPDX-License-Identifier: GPL-3.0-only

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <Style.h>

#include "AppIconProvider.h"
#include "PanelProperties.h"

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QT_WAYLAND_SHELL_INTEGRATION"))
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", Chalk::PanelProperties::ShellKey);

    const QString executable = QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath();
    QCoreApplication::addLibraryPath(QDir::cleanPath(executable + QStringLiteral("/../../plugins")));
    QCoreApplication::addLibraryPath(QDir::cleanPath(executable + QStringLiteral("/../plugins")));

    QGuiApplication app(argc, argv);
    QFont font = app.font();
    font.setStyleStrategy(QFont::StyleStrategy(font.styleStrategy() | QFont::NoSubpixelAntialias));
    app.setFont(font);
    app.setApplicationName(QStringLiteral("chalk"));
    app.setApplicationVersion(QStringLiteral(CHALK_VERSION));
    app.setQuitOnLastWindowClosed(true);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Panels of SanarDE"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption calendar(QStringLiteral("calendar"), QStringLiteral("Open the calendar at start"));
    parser.addOption(calendar);
    const QCommandLineOption launcher(QStringLiteral("launcher"), QStringLiteral("Open the launcher at start"));
    parser.addOption(launcher);
    parser.process(app);

    SanarStyle::init();

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.addImageProvider(QStringLiteral("appicon"), new Chalk::AppIconProvider);
    engine.setInitialProperties({{"openCalendar", parser.isSet(calendar)}, {"openLauncher", parser.isSet(launcher)}});
    engine.loadFromModule("Sanar.Chalk", "Bar");

    return app.exec();
}
