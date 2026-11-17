// SPDX-License-Identifier: GPL-3.0-only

#include <QtTest>

#include "SlateState.h"

using namespace Chalk;

class SlateStateTest : public QObject
{
    Q_OBJECT

private slots:
    void parsesTheWorkspacesAndTheFocusedTitle();
    void ignoresAnAcknowledgementWithoutState();
    void rejectsErrorsAndGarbage();
    void leavesTheTitleEmptyWhenNothingIsFocused();
    void buildsTheAppNameFromTheAppId();
    void listsTheWindowsWithTheirState();
};

void SlateStateTest::parsesTheWorkspacesAndTheFocusedTitle()
{
    const QByteArray line = R"({"ok":true,"data":{"workspace":2,"focused":7,
        "workspaces":[{"index":1,"layout":"floating","windows":0,"active":false},
                      {"index":2,"layout":"grid","windows":3,"active":true}],
        "windows":[{"id":5,"title":"one","focused":false},{"id":7,"title":"Terminal","app_id":"org.gnome.Terminal","focused":true}],
        "outputs":[]}})";

    const SlateState state = parseState(line);
    QVERIFY(state.valid);
    QCOMPARE(state.workspace, 2);
    QCOMPARE(state.workspaces.size(), 2);
    QCOMPARE(state.workspaces[1], (WorkspaceInfo{.index = 2, .windows = 3, .active = true}));
    QCOMPARE(state.focusedTitle, QStringLiteral("Terminal"));
    QCOMPARE(state.focusedAppId, QStringLiteral("org.gnome.Terminal"));
}

void SlateStateTest::listsTheWindowsWithTheirState()
{
    const QByteArray line = R"({"ok":true,"data":{"workspace":1,"focused":2,
        "workspaces":[{"index":1,"layout":"floating","windows":2,"active":true}],
        "windows":[{"id":1,"title":"Notes","app_id":"notes","workspace":1,"focused":false,"minimized":true},
                   {"id":2,"title":"Shell","app_id":"term","workspace":1,"focused":true,"minimized":false},
                   {"title":"broken"}],
        "outputs":[]}})";

    const SlateState state = parseState(line);
    QCOMPARE(state.windows.size(), 2);
    QCOMPARE(state.windows[0].id, quint64(1));
    QVERIFY(state.windows[0].minimized);
    QVERIFY(!state.windows[0].focused);
    QCOMPARE(state.windows[1].appId, QStringLiteral("term"));
    QVERIFY(state.windows[1].focused);
    QCOMPARE(state.windows[1].workspace, 1);
}

void SlateStateTest::ignoresAnAcknowledgementWithoutState()
{
    QVERIFY(!parseState(R"({"ok":true,"data":null})").valid);
    QVERIFY(!parseState(R"({"ok":true})").valid);
}

void SlateStateTest::rejectsErrorsAndGarbage()
{
    QVERIFY(!parseState(R"({"ok":false,"error":"no"})").valid);
    QVERIFY(!parseState("not json").valid);
    QVERIFY(!parseState("").valid);
    QVERIFY(!parseState("[1,2]").valid);
}

void SlateStateTest::leavesTheTitleEmptyWhenNothingIsFocused()
{
    const SlateState state = parseState(
        R"({"ok":true,"data":{"workspace":1,"workspaces":[{"index":1,"windows":0,"active":true}],"windows":[]}})");
    QVERIFY(state.valid);
    QVERIFY(state.focusedTitle.isEmpty());
}

void SlateStateTest::buildsTheAppNameFromTheAppId()
{
    QCOMPARE(appDisplayName("org.gnome.Calculator", "x"), QStringLiteral("Calculator"));
    QCOMPARE(appDisplayName("firefox", "x"), QStringLiteral("Firefox"));
    QCOMPARE(appDisplayName("gnome-terminal", "x"), QStringLiteral("Gnome terminal"));
    QCOMPARE(appDisplayName("", "A title"), QStringLiteral("A title"));
    QCOMPARE(appDisplayName("", ""), QString());
}

QTEST_APPLESS_MAIN(SlateStateTest)

#include "SlateStateTest.moc"
