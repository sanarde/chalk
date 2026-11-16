// SPDX-License-Identifier: GPL-3.0-only

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <Style.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("chalk"));
    app.setApplicationVersion(QStringLiteral(CHALK_VERSION));
    app.setQuitOnLastWindowClosed(true);

    SanarStyle::init();

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.loadFromModule("Sanar.Chalk", "Bar");

    return app.exec();
}
