// SPDX-License-Identifier: GPL-3.0-only

#include "Calendar.h"

#include <QLocale>
#include <QVariantMap>

namespace Chalk {

QList<CalendarDay> buildMonth(int year, int month, const QDate &today, Qt::DayOfWeek firstDay)
{
    const QDate first(year, month, 1);
    if (!first.isValid())
        return {};

    const int offset = (first.dayOfWeek() - firstDay + 7) % 7;
    const QDate start = first.addDays(-offset);

    QList<CalendarDay> days;
    days.reserve(CalendarCells);
    for (int index = 0; index < CalendarCells; ++index) {
        const QDate date = start.addDays(index);
        days.append({
            .date = date,
            .inMonth = date.month() == month && date.year() == year,
            .today = date == today,
            .weekend = date.dayOfWeek() >= Qt::Saturday,
        });
    }
    return days;
}

Calendar::Calendar(QObject *parent)
    : QObject(parent)
    , m_year(QDate::currentDate().year())
    , m_month(QDate::currentDate().month())
{
}

int Calendar::year() const
{
    return m_year;
}

int Calendar::month() const
{
    return m_month;
}

static QString capitalized(const QString &text)
{
    return text.isEmpty() ? text : text.at(0).toUpper() + text.mid(1);
}

QString Calendar::monthName() const
{
    return capitalized(QLocale().standaloneMonthName(m_month, QLocale::LongFormat));
}

QStringList Calendar::months() const
{
    const QLocale locale;
    QStringList names;
    for (int month = 1; month <= 12; ++month)
        names.append(capitalized(locale.standaloneMonthName(month, QLocale::LongFormat)));
    return names;
}

QString Calendar::title() const
{
    const QString month = monthName();
    if (month.isEmpty())
        return {};
    return month + QLatin1Char(' ') + QString::number(m_year);
}

QVariantList Calendar::days() const
{
    QVariantList list;
    const QLocale locale;
    for (const CalendarDay &day : buildMonth(m_year, m_month, QDate::currentDate(), locale.firstDayOfWeek())) {
        list.append(QVariantMap{
            {"day", day.date.day()},
            {"inMonth", day.inMonth},
            {"today", day.today},
            {"weekend", day.weekend},
        });
    }
    return list;
}

QStringList Calendar::weekdays() const
{
    const QLocale locale;
    QStringList names;
    int day = locale.firstDayOfWeek();
    for (int index = 0; index < 7; ++index) {
        names.append(locale.dayName(day, QLocale::NarrowFormat));
        day = day % 7 + 1;
    }
    return names;
}

void Calendar::next()
{
    shift(1);
}

void Calendar::previous()
{
    shift(-1);
}

void Calendar::today()
{
    const QDate now = QDate::currentDate();
    m_year = now.year();
    m_month = now.month();
    emit changed();
}

void Calendar::show(int year, int month)
{
    if (!QDate(year, month, 1).isValid())
        return;
    m_year = year;
    m_month = month;
    emit changed();
}

void Calendar::shift(int months)
{
    const QDate moved = QDate(m_year, m_month, 1).addMonths(months);
    m_year = moved.year();
    m_month = moved.month();
    emit changed();
}

}
