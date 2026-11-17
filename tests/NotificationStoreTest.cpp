// SPDX-License-Identifier: GPL-3.0-only

#include <QSignalSpy>
#include <QtTest>

#include "NotificationStore.h"

using namespace Chalk;

class NotificationStoreTest : public QObject
{
    Q_OBJECT

private slots:
    void idsGrowAndTheNewestComesFirst();
    void replacingKeepsTheIdAndMovesTheEntryToTheFront();
    void replacingAnUnknownIdCreatesANewEntry();
    void historyIsLimited();
    void timeoutRules_data();
    void timeoutRules();
    void actionsAreKeptInPairs();
    void urgencyIsClamped();
    void oversizedTextIsTrimmed();
    void bannerExpiresButStaysInTheHistory();
    void hidingTheBannerReportsDismissalOnce();
    void removingReportsTheCloseReasonOnlyForLiveEntries();
    void invokingChecksTheKeyAndHidesTheBanner();
    void clearingReportsEveryLiveEntry();
    void doNotDisturbKeepsQuietNotificationsInTheHistoryOnly();
};

static uint post(NotificationStore &store, const QString &summary, uint replaces = 0, int timeout = 0,
                 const QVariantMap &hints = {}, const QStringList &actions = {})
{
    return store.notify(QStringLiteral("app"), replaces, summary, QStringLiteral("body"), actions, hints, timeout);
}

void NotificationStoreTest::idsGrowAndTheNewestComesFirst()
{
    NotificationStore store;
    const uint first = post(store, "one");
    const uint second = post(store, "two");
    QVERIFY(second > first);
    QCOMPARE(store.count(), 2);
    QCOMPARE(store.data(store.index(0), NotificationStore::SummaryRole).toString(), QStringLiteral("two"));
    QCOMPARE(store.banner().value("id").toUInt(), second);
}

void NotificationStoreTest::replacingKeepsTheIdAndMovesTheEntryToTheFront()
{
    NotificationStore store;
    const uint first = post(store, "one");
    post(store, "two");
    QCOMPARE(post(store, "uno", first), first);
    QCOMPARE(store.count(), 2);
    QCOMPARE(store.data(store.index(0), NotificationStore::SummaryRole).toString(), QStringLiteral("uno"));
}

void NotificationStoreTest::replacingAnUnknownIdCreatesANewEntry()
{
    NotificationStore store;
    const uint id = post(store, "one", 4242);
    QVERIFY(id != 4242);
    QCOMPARE(store.count(), 1);
}

void NotificationStoreTest::historyIsLimited()
{
    NotificationStore store;
    QSignalSpy closed(&store, &NotificationStore::closed);
    for (int index = 0; index < NotificationStore::MaxEntries + 5; ++index)
        post(store, QString::number(index));
    QCOMPARE(store.count(), NotificationStore::MaxEntries);
    QCOMPARE(closed.size(), 5);
    QCOMPARE(closed.first().at(1).toUInt(), static_cast<uint>(NotificationStore::Undefined));
}

void NotificationStoreTest::timeoutRules_data()
{
    QTest::addColumn<int>("expire");
    QTest::addColumn<int>("urgency");
    QTest::addColumn<int>("expected");

    QTest::newRow("server default") << -1 << 1 << NotificationStore::DefaultTimeout;
    QTest::newRow("explicit") << 3000 << 0 << 3000;
    QTest::newRow("sticky") << 0 << 1 << 0;
    QTest::newRow("critical never expires") << 3000 << 2 << 0;
}

void NotificationStoreTest::timeoutRules()
{
    QFETCH(int, expire);
    QFETCH(int, urgency);
    QFETCH(int, expected);
    QCOMPARE(NotificationStore::bannerTimeout(expire, urgency), expected);
}

void NotificationStoreTest::actionsAreKeptInPairs()
{
    QCOMPARE(NotificationStore::sanitizedActions({"a", "A", "b"}), QStringList({"a", "A"}));
    QCOMPARE(NotificationStore::sanitizedActions({"", "x", "k", "K"}), QStringList({"k", "K"}));
    QCOMPARE(NotificationStore::sanitizedActions({}).size(), 0);
    QStringList many;
    for (int index = 0; index < 40; ++index)
        many << QString::number(index);
    QCOMPARE(NotificationStore::sanitizedActions(many).size(), 6);
}

void NotificationStoreTest::urgencyIsClamped()
{
    NotificationStore store;
    post(store, "x", 0, 0, {{"urgency", 9}});
    QCOMPARE(store.data(store.index(0), NotificationStore::UrgencyRole).toInt(), 2);
    post(store, "y", 0, 0, {{"urgency", -3}});
    QCOMPARE(store.data(store.index(0), NotificationStore::UrgencyRole).toInt(), 0);
    post(store, "z");
    QCOMPARE(store.data(store.index(0), NotificationStore::UrgencyRole).toInt(), 1);
}

void NotificationStoreTest::oversizedTextIsTrimmed()
{
    NotificationStore store;
    store.notify(QString(500, 'a'), 0, QString(5000, 'b'), QString(5000, 'c'), {}, {}, 0);
    QCOMPARE(store.data(store.index(0), NotificationStore::SummaryRole).toString().size(), NotificationStore::MaxText);
    QCOMPARE(store.data(store.index(0), NotificationStore::BodyRole).toString().size(), NotificationStore::MaxText);
    QCOMPARE(store.data(store.index(0), NotificationStore::AppRole).toString().size(), 100);
}

void NotificationStoreTest::bannerExpiresButStaysInTheHistory()
{
    NotificationStore store;
    QSignalSpy closed(&store, &NotificationStore::closed);
    const uint id = post(store, "soon", 0, 30);
    QVERIFY(!store.banner().isEmpty());
    QTRY_VERIFY(store.banner().isEmpty());
    QCOMPARE(store.count(), 1);
    QCOMPARE(closed.size(), 1);
    QCOMPARE(closed.first().at(0).toUInt(), id);
    QCOMPARE(closed.first().at(1).toUInt(), static_cast<uint>(NotificationStore::Expired));
    QVERIFY(!store.data(store.index(0), NotificationStore::LiveRole).toBool());
}

void NotificationStoreTest::hidingTheBannerReportsDismissalOnce()
{
    NotificationStore store;
    QSignalSpy closed(&store, &NotificationStore::closed);
    post(store, "x");
    store.hideBanner();
    store.hideBanner();
    QVERIFY(store.banner().isEmpty());
    QCOMPARE(closed.size(), 1);
    QCOMPARE(closed.first().at(1).toUInt(), static_cast<uint>(NotificationStore::Dismissed));
}

void NotificationStoreTest::removingReportsTheCloseReasonOnlyForLiveEntries()
{
    NotificationStore store;
    QSignalSpy closed(&store, &NotificationStore::closed);
    const uint live = post(store, "live");
    store.close(live, NotificationStore::ClosedByCall);
    QCOMPARE(closed.size(), 1);
    QCOMPARE(closed.first().at(1).toUInt(), static_cast<uint>(NotificationStore::ClosedByCall));

    const uint dead = post(store, "dead");
    store.hideBanner();
    closed.clear();
    store.remove(dead);
    QCOMPARE(closed.size(), 0);
    QCOMPARE(store.count(), 0);
    store.remove(9999);
}

void NotificationStoreTest::invokingChecksTheKeyAndHidesTheBanner()
{
    NotificationStore store;
    QSignalSpy invoked(&store, &NotificationStore::actionInvoked);
    const uint id = post(store, "x", 0, 0, {}, {"open", "Open"});
    store.invoke(id, "other");
    QCOMPARE(invoked.size(), 0);
    QVERIFY(!store.banner().isEmpty());
    store.invoke(id, "open");
    QCOMPARE(invoked.size(), 1);
    QVERIFY(store.banner().isEmpty());
    store.invoke(id, "open");
    QCOMPARE(invoked.size(), 1);
}

void NotificationStoreTest::clearingReportsEveryLiveEntry()
{
    NotificationStore store;
    QSignalSpy closed(&store, &NotificationStore::closed);
    post(store, "a");
    post(store, "b");
    store.clear();
    QCOMPARE(store.count(), 0);
    QCOMPARE(closed.size(), 2);
    QVERIFY(store.banner().isEmpty());
}

void NotificationStoreTest::doNotDisturbKeepsQuietNotificationsInTheHistoryOnly()
{
    NotificationStore store;
    QSignalSpy closed(&store, &NotificationStore::closed);
    store.setDoNotDisturb(true);
    post(store, "quiet");
    QVERIFY(store.banner().isEmpty());
    QCOMPARE(store.count(), 1);
    QVERIFY(!store.data(store.index(0), NotificationStore::LiveRole).toBool());
    QCOMPARE(closed.size(), 1);

    post(store, "urgent", 0, 0, {{"urgency", 2}});
    QVERIFY(!store.banner().isEmpty());

    store.setDoNotDisturb(false);
    post(store, "loud");
    QCOMPARE(store.banner().value("summary").toString(), QStringLiteral("loud"));
}

QTEST_GUILESS_MAIN(NotificationStoreTest)

#include "NotificationStoreTest.moc"
