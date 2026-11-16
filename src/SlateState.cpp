// SPDX-License-Identifier: GPL-3.0-only

#include "SlateState.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace Chalk {

SlateState parseState(const QByteArray &line)
{
    const QJsonDocument document = QJsonDocument::fromJson(line);
    if (!document.isObject() || !document.object().value("ok").toBool())
        return {};

    const QJsonObject data = document.object().value("data").toObject();
    if (!data.contains("workspaces"))
        return {};

    SlateState state;
    state.valid = true;
    state.workspace = data.value("workspace").toInt();

    for (const QJsonValue &value : data.value("workspaces").toArray()) {
        const QJsonObject workspace = value.toObject();
        state.workspaces.append({
            .index = workspace.value("index").toInt(),
            .windows = workspace.value("windows").toInt(),
            .active = workspace.value("active").toBool(),
        });
    }

    for (const QJsonValue &value : data.value("windows").toArray()) {
        const QJsonObject window = value.toObject();
        if (window.value("focused").toBool()) {
            state.focusedTitle = window.value("title").toString();
            break;
        }
    }
    return state;
}

}
