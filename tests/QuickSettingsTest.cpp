// SPDX-License-Identifier: GPL-3.0-only

#include <QtTest>

#include "QuickSettings.h"

using namespace Chalk;

class QuickSettingsTest : public QObject
{
    Q_OBJECT

private slots:
    void parsesTheVolume_data();
    void parsesTheVolume();
    void findsTheActiveNetwork_data();
    void findsTheActiveNetwork();
    void clampsToTheUnitRange();
};

void QuickSettingsTest::parsesTheVolume_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<double>("volume");
    QTest::addColumn<bool>("muted");

    QTest::newRow("plain") << "Volume: 0.43\n" << true << 0.43 << false;
    QTest::newRow("muted") << "Volume: 0.80 [MUTED]\n" << true << 0.80 << true;
    QTest::newRow("boosted is clamped") << "Volume: 1.50\n" << true << 1.0 << false;
    QTest::newRow("empty") << "" << false << 0.0 << false;
    QTest::newRow("garbage") << "Volume: loud" << false << 0.0 << false;
    QTest::newRow("negative") << "Volume: -1.0" << false << 0.0 << false;
    QTest::newRow("another command") << "Node: 12" << false << 0.0 << false;
}

void QuickSettingsTest::parsesTheVolume()
{
    QFETCH(QString, text);
    QFETCH(bool, valid);
    QFETCH(double, volume);
    QFETCH(bool, muted);

    const VolumeState state = QuickSettings::parseVolume(text);
    QCOMPARE(state.valid, valid);
    QVERIFY(qAbs(state.volume - volume) < 1e-9);
    QCOMPARE(state.muted, muted);
}

void QuickSettingsTest::findsTheActiveNetwork_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("name");

    QTest::newRow("second line is active") << "no:Cafe\nyes:Home\nno:Other\n" << "Home";
    QTest::newRow("none") << "no:Cafe\n" << "";
    QTest::newRow("colon in the name is escaped") << "yes:My\\:Net\n" << "My:Net";
    QTest::newRow("backslash in the name") << "yes:A\\\\B\n" << "A\\B";
    QTest::newRow("empty") << "" << "";
    QTest::newRow("dangling escape") << "yes:abc\\" << "abc";
}

void QuickSettingsTest::findsTheActiveNetwork()
{
    QFETCH(QString, text);
    QFETCH(QString, name);
    QCOMPARE(QuickSettings::parseActiveNetwork(text), name);
}

void QuickSettingsTest::clampsToTheUnitRange()
{
    QCOMPARE(QuickSettings::clampUnit(-3), 0.0);
    QCOMPARE(QuickSettings::clampUnit(0.4), 0.4);
    QCOMPARE(QuickSettings::clampUnit(7), 1.0);
}

QTEST_GUILESS_MAIN(QuickSettingsTest)

#include "QuickSettingsTest.moc"
