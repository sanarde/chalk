// SPDX-License-Identifier: GPL-3.0-only

#include "AppCatalog.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QLocale>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <algorithm>

namespace Chalk {

namespace {

QString unescaped(const QString &value)
{
    QString out;
    out.reserve(value.size());
    for (int i = 0; i < value.size(); ++i) {
        if (value[i] == QLatin1Char('\\') && i + 1 < value.size()) {
            ++i;
            switch (value[i].unicode()) {
            case 's': out += QLatin1Char(' '); break;
            case 'n': out += QLatin1Char('\n'); break;
            case 't': out += QLatin1Char('\t'); break;
            case 'r': out += QLatin1Char('\r'); break;
            default: out += value[i]; break;
            }
        } else {
            out += value[i];
        }
    }
    return out;
}

bool isTrue(const QString &value)
{
    return value.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0;
}

QStringList splitList(const QString &value)
{
    QStringList out;
    for (const QString &part : value.split(QLatin1Char(';'), Qt::SkipEmptyParts))
        out << part.trimmed();
    return out;
}

}

AppCatalog::AppCatalog(QObject *parent)
    : QAbstractListModel(parent)
{
    reload();
}

int AppCatalog::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_shown.size();
}

QVariant AppCatalog::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_shown.size())
        return {};
    const AppEntry &entry = m_shown.at(index.row());
    switch (role) {
    case NameRole: return entry.name;
    case CommentRole: return entry.comment;
    case IconRole: return entry.icon;
    case IdRole: return entry.id;
    default: return {};
    }
}

QHash<int, QByteArray> AppCatalog::roleNames() const
{
    return {{NameRole, "name"}, {CommentRole, "comment"}, {IconRole, "icon"}, {IdRole, "appId"}};
}

QString AppCatalog::query() const
{
    return m_query;
}

void AppCatalog::setQuery(const QString &query)
{
    if (m_query == query)
        return;
    m_query = query;
    applyFilter();
    emit queryChanged();
}

int AppCatalog::count() const
{
    return m_shown.size();
}

int AppCatalog::total() const
{
    return m_all.size();
}

void AppCatalog::setEntries(const QList<AppEntry> &entries)
{
    m_all = entries;
    applyFilter();
    emit totalChanged();
}

void AppCatalog::applyFilter()
{
    beginResetModel();
    m_shown = filtered(m_all, m_query);
    endResetModel();
    emit countChanged();
}

QStringList AppCatalog::localeCandidates(const QString &locale)
{
    QString base = locale;
    const int dot = base.indexOf(QLatin1Char('.'));
    if (dot >= 0)
        base.truncate(dot);
    const int at = base.indexOf(QLatin1Char('@'));
    if (at >= 0)
        base.truncate(at);
    if (base.isEmpty() || base == QLatin1String("C") || base == QLatin1String("POSIX"))
        return {};
    QStringList out{base};
    const int underscore = base.indexOf(QLatin1Char('_'));
    if (underscore > 0)
        out << base.left(underscore);
    return out;
}

AppEntry AppCatalog::parseDesktopFile(const QString &id, const QString &text, const QStringList &languages,
                                      const QStringList &desktops, bool *visible)
{
    AppEntry entry;
    entry.id = id;
    bool inEntry = false;
    bool application = false;
    bool hidden = false;
    bool terminal = false;
    QStringList only;
    QStringList not_;
    QHash<QString, QString> localizedName;
    QHash<QString, QString> localizedComment;
    QHash<QString, QString> localizedKeywords;
    QString keywords;

    const QStringList lines = text.split(QLatin1Char('\n'));
    for (const QString &raw : lines) {
        const QString line = raw.trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#')))
            continue;
        if (line.startsWith(QLatin1Char('['))) {
            inEntry = line == QLatin1String("[Desktop Entry]");
            continue;
        }
        if (!inEntry)
            continue;
        const int eq = line.indexOf(QLatin1Char('='));
        if (eq <= 0)
            continue;
        const QString key = line.left(eq).trimmed();
        const QString value = line.mid(eq + 1).trimmed();

        if (key == QLatin1String("Type")) {
            application = value == QLatin1String("Application");
        } else if (key == QLatin1String("Name")) {
            entry.name = unescaped(value);
        } else if (key.startsWith(QLatin1String("Name["))) {
            localizedName.insert(key.mid(5, key.size() - 6), unescaped(value));
        } else if (key == QLatin1String("Comment")) {
            entry.comment = unescaped(value);
        } else if (key.startsWith(QLatin1String("Comment["))) {
            localizedComment.insert(key.mid(8, key.size() - 9), unescaped(value));
        } else if (key == QLatin1String("Keywords")) {
            keywords = value;
        } else if (key.startsWith(QLatin1String("Keywords["))) {
            localizedKeywords.insert(key.mid(9, key.size() - 10), value);
        } else if (key == QLatin1String("Icon")) {
            entry.icon = value;
        } else if (key == QLatin1String("Exec")) {
            entry.exec = value;
        } else if (key == QLatin1String("NoDisplay") || key == QLatin1String("Hidden")) {
            hidden = hidden || isTrue(value);
        } else if (key == QLatin1String("Terminal")) {
            terminal = isTrue(value);
        } else if (key == QLatin1String("OnlyShowIn")) {
            only = splitList(value);
        } else if (key == QLatin1String("NotShowIn")) {
            not_ = splitList(value);
        }
    }

    for (const QString &language : languages) {
        if (localizedName.contains(language)) {
            entry.name = localizedName.value(language);
            break;
        }
    }
    for (const QString &language : languages) {
        if (localizedComment.contains(language)) {
            entry.comment = localizedComment.value(language);
            break;
        }
    }
    entry.keywords = splitList(keywords);
    for (const QString &language : languages) {
        if (localizedKeywords.contains(language)) {
            entry.keywords << splitList(localizedKeywords.value(language));
            break;
        }
    }

    bool show = application && !hidden && !terminal && !entry.name.isEmpty() && !entry.exec.isEmpty();
    if (show && !only.isEmpty()) {
        show = std::any_of(desktops.begin(), desktops.end(), [&](const QString &d) { return only.contains(d); });
    }
    if (show && std::any_of(desktops.begin(), desktops.end(), [&](const QString &d) { return not_.contains(d); }))
        show = false;
    if (visible)
        *visible = show;
    return entry;
}

QStringList AppCatalog::commandLine(const QString &exec)
{
    QString cleaned;
    cleaned.reserve(exec.size());
    for (int i = 0; i < exec.size(); ++i) {
        if (exec[i] != QLatin1Char('%') || i + 1 >= exec.size()) {
            cleaned += exec[i];
            continue;
        }
        ++i;
        if (exec[i] == QLatin1Char('%'))
            cleaned += QLatin1Char('%');
    }
    QStringList parts = QProcess::splitCommand(cleaned);
    return parts;
}

double AppCatalog::score(const AppEntry &entry, const QString &query)
{
    const QString needle = query.trimmed();
    if (needle.isEmpty())
        return 1.0;
    const QString name = entry.name;
    if (name.compare(needle, Qt::CaseInsensitive) == 0)
        return 5.0;
    if (name.startsWith(needle, Qt::CaseInsensitive))
        return 4.0;
    const QStringList words = name.split(QRegularExpression(QStringLiteral("[\\s\\-_.]+")), Qt::SkipEmptyParts);
    for (const QString &word : words) {
        if (word.startsWith(needle, Qt::CaseInsensitive))
            return 3.0;
    }
    if (name.contains(needle, Qt::CaseInsensitive))
        return 2.0;
    for (const QString &keyword : entry.keywords) {
        if (keyword.contains(needle, Qt::CaseInsensitive))
            return 1.5;
    }
    if (entry.id.contains(needle, Qt::CaseInsensitive))
        return 1.2;
    if (entry.comment.contains(needle, Qt::CaseInsensitive))
        return 1.0;
    return 0.0;
}

QList<AppEntry> AppCatalog::filtered(const QList<AppEntry> &entries, const QString &query)
{
    QList<QPair<double, AppEntry>> ranked;
    for (const AppEntry &entry : entries) {
        const double value = score(entry, query);
        if (value > 0.0)
            ranked.append({value, entry});
    }
    std::stable_sort(ranked.begin(), ranked.end(), [](const auto &a, const auto &b) {
        if (a.first != b.first)
            return a.first > b.first;
        return a.second.name.compare(b.second.name, Qt::CaseInsensitive) < 0;
    });
    QList<AppEntry> out;
    out.reserve(ranked.size());
    for (const auto &item : ranked)
        out.append(item.second);
    return out;
}

void AppCatalog::reload()
{
    const QStringList languages = localeCandidates(QLocale().name());
    const QStringList desktops = qEnvironmentVariable("XDG_CURRENT_DESKTOP").split(QLatin1Char(':'), Qt::SkipEmptyParts);
    const QStringList roots = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);

    QHash<QString, bool> seen;
    QList<AppEntry> entries;
    for (const QString &root : roots) {
        const QDir base(root);
        QDirIterator it(root, {QStringLiteral("*.desktop")}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString path = it.next();
            QString id = base.relativeFilePath(path);
            id.replace(QLatin1Char('/'), QLatin1Char('-'));
            if (seen.contains(id))
                continue;
            seen.insert(id, true);
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly))
                continue;
            bool visible = false;
            const AppEntry entry = parseDesktopFile(id, QString::fromUtf8(file.read(1 << 20)), languages, desktops, &visible);
            if (visible)
                entries.append(entry);
        }
    }
    setEntries(entries);
}

bool AppCatalog::launch(int row)
{
    if (row < 0 || row >= m_shown.size())
        return false;
    const QStringList parts = commandLine(m_shown.at(row).exec);
    if (parts.isEmpty())
        return false;
    return QProcess::startDetached(parts.first(), parts.mid(1));
}

}
