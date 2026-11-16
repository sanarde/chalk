// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QDate>
#include <QList>
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

namespace Chalk {

struct CalendarDay {
    QDate date;
    bool inMonth = false;
    bool today = false;
    bool weekend = false;

    bool operator==(const CalendarDay &other) const = default;
};

constexpr int CalendarCells = 42;

QList<CalendarDay> buildMonth(int year, int month, const QDate &today, Qt::DayOfWeek firstDay);

class Calendar : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int year READ year NOTIFY changed FINAL)
    Q_PROPERTY(int month READ month NOTIFY changed FINAL)
    Q_PROPERTY(QString title READ title NOTIFY changed FINAL)
    Q_PROPERTY(QVariantList days READ days NOTIFY changed FINAL)
    Q_PROPERTY(QStringList weekdays READ weekdays NOTIFY changed FINAL)
    Q_PROPERTY(QStringList months READ months CONSTANT FINAL)
    Q_PROPERTY(QString monthName READ monthName NOTIFY changed FINAL)

public:
    explicit Calendar(QObject *parent = nullptr);

    int year() const;
    int month() const;
    QString title() const;
    QVariantList days() const;
    QStringList weekdays() const;
    QStringList months() const;
    QString monthName() const;

    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void today();
    Q_INVOKABLE void show(int year, int month);

signals:
    void changed();

private:
    void shift(int months);

    int m_year;
    int m_month;
};

}
