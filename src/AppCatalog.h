// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

namespace Chalk {

struct AppEntry
{
    QString id;
    QString name;
    QString comment;
    QString icon;
    QString exec;
    QStringList keywords;
};

class AppCatalog : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged FINAL)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int total READ total NOTIFY totalChanged FINAL)

public:
    enum Role { NameRole = Qt::UserRole + 1, CommentRole, IconRole, IdRole };

    explicit AppCatalog(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString query() const;
    void setQuery(const QString &query);
    int count() const;
    int total() const;

    void setEntries(const QList<AppEntry> &entries);

    Q_INVOKABLE void reload();
    Q_INVOKABLE bool launch(int row);

    static AppEntry parseDesktopFile(const QString &id, const QString &text, const QStringList &languages,
                                     const QStringList &desktops, bool *visible);
    static QStringList commandLine(const QString &exec);
    static double score(const AppEntry &entry, const QString &query);
    static QList<AppEntry> filtered(const QList<AppEntry> &entries, const QString &query);
    static QStringList localeCandidates(const QString &locale);

signals:
    void queryChanged();
    void countChanged();
    void totalChanged();

private:
    void applyFilter();

    QList<AppEntry> m_all;
    QList<AppEntry> m_shown;
    QString m_query;
};

}
