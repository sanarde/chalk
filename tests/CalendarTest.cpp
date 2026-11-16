// SPDX-License-Identifier: GPL-3.0-only

#include <QtTest>

#include "Calendar.h"

using namespace Chalk;

class CalendarTest : public QObject
{
    Q_OBJECT

private slots:
    void alwaysBuildsSixWeeks();
    void startsOnTheFirstDayOfTheWeek_data();
    void startsOnTheFirstDayOfTheWeek();
    void marksTodayTheMonthAndTheWeekend();
    void handlesLeapDaysAndYearEnds();
    void rejectsInvalidMonths();
    void jumpsToAMonthAndIgnoresInvalidOnes();
    void listsTwelveCapitalizedMonths();
};

void CalendarTest::alwaysBuildsSixWeeks()
{
    for (int month = 1; month <= 12; ++month)
        QCOMPARE(buildMonth(2026, month, QDate(2026, 1, 1), Qt::Monday).size(), CalendarCells);
}

void CalendarTest::startsOnTheFirstDayOfTheWeek_data()
{
    QTest::addColumn<int>("firstDay");
    QTest::addColumn<QDate>("expectedStart");

    QTest::newRow("monday") << int(Qt::Monday) << QDate(2026, 10, 26);
    QTest::newRow("sunday") << int(Qt::Sunday) << QDate(2026, 11, 1);
    QTest::newRow("saturday") << int(Qt::Saturday) << QDate(2026, 10, 31);
}

void CalendarTest::startsOnTheFirstDayOfTheWeek()
{
    QFETCH(int, firstDay);
    QFETCH(QDate, expectedStart);

    const QList<CalendarDay> days = buildMonth(2026, 11, QDate(2026, 11, 16), Qt::DayOfWeek(firstDay));
    QCOMPARE(days.first().date, expectedStart);
    QCOMPARE(days.first().date.dayOfWeek(), firstDay);
}

void CalendarTest::marksTodayTheMonthAndTheWeekend()
{
    const QList<CalendarDay> days = buildMonth(2026, 11, QDate(2026, 11, 16), Qt::Monday);

    int today = 0;
    int inMonth = 0;
    for (const CalendarDay &day : days) {
        today += day.today;
        inMonth += day.inMonth;
        QCOMPARE(day.weekend, day.date.dayOfWeek() >= 6);
    }
    QCOMPARE(today, 1);
    QCOMPARE(inMonth, 30);
    QVERIFY(!days.first().inMonth);
}

void CalendarTest::handlesLeapDaysAndYearEnds()
{
    int february = 0;
    for (const CalendarDay &day : buildMonth(2028, 2, QDate(2028, 2, 29), Qt::Monday))
        february += day.inMonth;
    QCOMPARE(february, 29);

    const QList<CalendarDay> december = buildMonth(2026, 12, QDate(2026, 12, 31), Qt::Monday);
    int inMonth = 0;
    for (const CalendarDay &day : december)
        inMonth += day.inMonth;
    QCOMPARE(inMonth, 31);
    QCOMPARE(december.last().date.year(), 2027);
}

void CalendarTest::rejectsInvalidMonths()
{
    QVERIFY(buildMonth(2026, 0, QDate(2026, 1, 1), Qt::Monday).isEmpty());
    QVERIFY(buildMonth(2026, 13, QDate(2026, 1, 1), Qt::Monday).isEmpty());
}

void CalendarTest::jumpsToAMonthAndIgnoresInvalidOnes()
{
    Calendar calendar;
    calendar.show(2031, 2);
    QCOMPARE(calendar.year(), 2031);
    QCOMPARE(calendar.month(), 2);
    calendar.show(2040, 13);
    calendar.show(2040, 0);
    QCOMPARE(calendar.year(), 2031);
    QCOMPARE(calendar.month(), 2);
}

void CalendarTest::listsTwelveCapitalizedMonths()
{
    Calendar calendar;
    const QStringList months = calendar.months();
    QCOMPARE(months.size(), 12);
    for (const QString &name : months)
        QVERIFY(!name.isEmpty() && name.at(0).isUpper());
}

QTEST_APPLESS_MAIN(CalendarTest)

#include "CalendarTest.moc"
