// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QQuickImageProvider>

namespace Chalk {

class AppIconProvider : public QQuickImageProvider
{
public:
    AppIconProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};

}
