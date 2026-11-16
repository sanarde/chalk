// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>
#include <QWindow>
#include <QtQml/qqmlregistration.h>

namespace Chalk {

class PanelShell : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QWindow *window READ window WRITE setWindow NOTIFY windowChanged FINAL)
    Q_PROPERTY(Edge edge READ edge WRITE setEdge NOTIFY edgeChanged FINAL)
    Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged FINAL)
    Q_PROPERTY(Kind kind READ kind WRITE setKind NOTIFY kindChanged FINAL)

public:
    enum Edge { Top, Bottom };
    Q_ENUM(Edge)

    enum Kind { Panel, Overlay };
    Q_ENUM(Kind)

    explicit PanelShell(QObject *parent = nullptr);

    QWindow *window() const;
    void setWindow(QWindow *window);
    Edge edge() const;
    void setEdge(Edge edge);
    int thickness() const;
    Kind kind() const;
    void setKind(Kind kind);
    void setThickness(int thickness);

    Q_INVOKABLE void show();

signals:
    void windowChanged();
    void edgeChanged();
    void thicknessChanged();
    void kindChanged();

private:
    void placeOnX11();
    void placeOnWayland();

    QWindow *m_window = nullptr;
    Edge m_edge = Top;
    int m_thickness = 40;
    Kind m_kind = Panel;
};

}
