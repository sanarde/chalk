// SPDX-License-Identifier: GPL-3.0-only

#include "AppIconProvider.h"

#include <QFileInfo>
#include <QIcon>

namespace Chalk {

AppIconProvider::AppIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap AppIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const int edge = requestedSize.width() > 0 ? requestedSize.width() : 48;
    QIcon icon;
    if (id.startsWith(QLatin1Char('/')) && QFileInfo::exists(id))
        icon = QIcon(id);
    else
        icon = QIcon::fromTheme(id);
    if (icon.isNull())
        return {};
    QPixmap pixmap = icon.pixmap(QSize(edge, edge));
    if (size)
        *size = pixmap.size();
    return pixmap;
}

}
