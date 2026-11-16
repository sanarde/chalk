// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

namespace Chalk {

struct WorkspaceInfo {
    int index = 0;
    int windows = 0;
    bool active = false;

    bool operator==(const WorkspaceInfo &other) const = default;
};

struct SlateState {
    bool valid = false;
    int workspace = 0;
    QList<WorkspaceInfo> workspaces;
    QString focusedTitle;
    QString focusedAppId;
};

SlateState parseState(const QByteArray &line);
QString appDisplayName(const QString &appId, const QString &title);

}
