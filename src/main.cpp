// SPDX-License-Identifier: GPL-3.0-only

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <Style.h>

#include "PanelProperties.h"

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QT_WAYLAND_SHELL_INTEGRATION"))
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", Chalk::PanelProperties::ShellKey);

    const QString executable = QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath();
    QCoreApplication::addLibraryPath(QDir::cleanPath(executable + QStringLiteral("/../../plugins")));
    QCoreApplication::addLibraryPath(QDir::cleanPath(executable + QStringLiteral("/../plugins")));

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("chalk"));
    app.setApplicationVersion(QStringLiteral(CHALK_VERSION));
    app.setQuitOnLastWindowClosed(true);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Panels of SanarDE"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption calendar(QStringLiteral("calendar"), QStringLiteral("Open the calendar at start"));
    parser.addOption(calendar);
    parser.process(app);

    SanarStyle::init();

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.setInitialProperties({{"openCalendar", parser.isSet(calendar)}});
    engine.loadFromModule("Sanar.Chalk", "Bar");

    return app.exec();
}
