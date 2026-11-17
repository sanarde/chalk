// SPDX-License-Identifier: GPL-3.0-only

#include <QtTest>

#include "AppCatalog.h"

using namespace Chalk;

class AppCatalogTest : public QObject
{
    Q_OBJECT

private slots:
    void parsesAnEntry();
    void prefersTheLocalizedName();
    void hidesWhatShouldNotBeListed_data();
    void hidesWhatShouldNotBeListed();
    void stripsFieldCodes_data();
    void stripsFieldCodes();
    void ranksTheBestMatchFirst();
    void emptyQueryKeepsEverythingSorted();
    void findsByKeywordAndComment();
    void listsLocaleCandidates();
    void modelFollowsTheQuery();
};

static const QString sample = QStringLiteral(
    "[Desktop Entry]\n"
    "Type=Application\n"
    "Name=Files\n"
    "Name[ru]=Файлы\n"
    "Comment=Browse files\n"
    "Keywords=folder;manager;\n"
    "Icon=folder\n"
    "Exec=nautilus --new-window %U\n"
    "[Desktop Action new]\n"
    "Name=Other\n");

void AppCatalogTest::parsesAnEntry()
{
    bool visible = false;
    const AppEntry entry = AppCatalog::parseDesktopFile("files.desktop", sample, {}, {}, &visible);
    QVERIFY(visible);
    QCOMPARE(entry.name, QString("Files"));
    QCOMPARE(entry.comment, QString("Browse files"));
    QCOMPARE(entry.icon, QString("folder"));
    QCOMPARE(entry.keywords, (QStringList{"folder", "manager"}));
}

void AppCatalogTest::prefersTheLocalizedName()
{
    bool visible = false;
    const AppEntry entry = AppCatalog::parseDesktopFile("files.desktop", sample, {"ru_RU", "ru"}, {}, &visible);
    QCOMPARE(entry.name, QString("Файлы"));
}

void AppCatalogTest::hidesWhatShouldNotBeListed_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QStringList>("desktops");
    QTest::addColumn<bool>("visible");

    const QString head = "[Desktop Entry]\nType=Application\nName=A\nExec=a\n";
    QTest::newRow("plain") << head << QStringList() << true;
    QTest::newRow("no display") << head + "NoDisplay=true\n" << QStringList() << false;
    QTest::newRow("hidden") << head + "Hidden=true\n" << QStringList() << false;
    QTest::newRow("terminal") << head + "Terminal=true\n" << QStringList() << false;
    QTest::newRow("link") << "[Desktop Entry]\nType=Link\nName=A\nExec=a\n" << QStringList() << false;
    QTest::newRow("no exec") << "[Desktop Entry]\nType=Application\nName=A\n" << QStringList() << false;
    QTest::newRow("only show in other") << head + "OnlyShowIn=KDE;\n" << QStringList{"Slate"} << false;
    QTest::newRow("only show in this") << head + "OnlyShowIn=KDE;Slate;\n" << QStringList{"Slate"} << true;
    QTest::newRow("not show in this") << head + "NotShowIn=Slate;\n" << QStringList{"Slate"} << false;
    QTest::newRow("only show in without desktop") << head + "OnlyShowIn=KDE;\n" << QStringList() << false;
}

void AppCatalogTest::hidesWhatShouldNotBeListed()
{
    QFETCH(QString, text);
    QFETCH(QStringList, desktops);
    QFETCH(bool, visible);

    bool shown = !visible;
    AppCatalog::parseDesktopFile("a.desktop", text, {}, desktops, &shown);
    QCOMPARE(shown, visible);
}

void AppCatalogTest::stripsFieldCodes_data()
{
    QTest::addColumn<QString>("exec");
    QTest::addColumn<QStringList>("parts");

    QTest::newRow("file code") << "nautilus --new-window %U" << QStringList{"nautilus", "--new-window"};
    QTest::newRow("quoted") << "\"/opt/My App/run\" --flag %f" << QStringList{"/opt/My App/run", "--flag"};
    QTest::newRow("percent") << "echo 100%%" << QStringList{"echo", "100%"};
    QTest::newRow("trailing percent") << "echo 5%" << QStringList{"echo", "5%"};
    QTest::newRow("empty") << "" << QStringList();
}

void AppCatalogTest::stripsFieldCodes()
{
    QFETCH(QString, exec);
    QFETCH(QStringList, parts);
    QCOMPARE(AppCatalog::commandLine(exec), parts);
}

static AppEntry app(const QString &name, const QString &comment = {}, const QStringList &keywords = {})
{
    AppEntry entry;
    entry.id = name.toLower() + ".desktop";
    entry.name = name;
    entry.comment = comment;
    entry.keywords = keywords;
    return entry;
}

void AppCatalogTest::ranksTheBestMatchFirst()
{
    const QList<AppEntry> all{app("Text Editor"), app("Terminal Emulator"), app("Teams"), app("Settings")};
    const QList<AppEntry> out = AppCatalog::filtered(all, "te");
    QCOMPARE(out.size(), 3);
    QCOMPARE(out.at(0).name, QString("Teams"));
    QCOMPARE(out.at(1).name, QString("Terminal Emulator"));
    QCOMPARE(out.at(2).name, QString("Text Editor"));

    const QList<AppEntry> exact = AppCatalog::filtered(all, "teams");
    QCOMPARE(exact.size(), 1);
}

void AppCatalogTest::emptyQueryKeepsEverythingSorted()
{
    const QList<AppEntry> all{app("zeta"), app("Alpha"), app("beta")};
    const QList<AppEntry> out = AppCatalog::filtered(all, "  ");
    QCOMPARE(out.size(), 3);
    QCOMPARE(out.at(0).name, QString("Alpha"));
    QCOMPARE(out.at(1).name, QString("beta"));
    QCOMPARE(out.at(2).name, QString("zeta"));
}

void AppCatalogTest::findsByKeywordAndComment()
{
    const QList<AppEntry> all{app("Files", "Browse the disk", {"folder"}), app("Clock")};
    QCOMPARE(AppCatalog::filtered(all, "folder").size(), 1);
    QCOMPARE(AppCatalog::filtered(all, "disk").size(), 1);
    QCOMPARE(AppCatalog::filtered(all, "nothing").size(), 0);
    QVERIFY(AppCatalog::score(all.at(0), "folder") > AppCatalog::score(all.at(0), "disk"));
}

void AppCatalogTest::listsLocaleCandidates()
{
    QCOMPARE(AppCatalog::localeCandidates("ru_RU.UTF-8"), (QStringList{"ru_RU", "ru"}));
    QCOMPARE(AppCatalog::localeCandidates("de"), (QStringList{"de"}));
    QCOMPARE(AppCatalog::localeCandidates("sr_RS@latin"), (QStringList{"sr_RS", "sr"}));
    QCOMPARE(AppCatalog::localeCandidates("C"), QStringList());
    QCOMPARE(AppCatalog::localeCandidates(""), QStringList());
}

void AppCatalogTest::modelFollowsTheQuery()
{
    AppCatalog catalog;
    catalog.setEntries({app("Alpha"), app("Beta")});
    QCOMPARE(catalog.count(), 2);
    QSignalSpy spy(&catalog, &AppCatalog::countChanged);
    catalog.setQuery("bet");
    QCOMPARE(catalog.count(), 1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(catalog.index(0, 0).data(AppCatalog::NameRole).toString(), QString("Beta"));
    QVERIFY(!catalog.launch(5));
    QVERIFY(!catalog.launch(-1));
    QCOMPARE(catalog.total(), 2);
}

QTEST_GUILESS_MAIN(AppCatalogTest)
#include "AppCatalogTest.moc"
