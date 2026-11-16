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
};

void SlateStateTest::parsesTheWorkspacesAndTheFocusedTitle()
{
    const QByteArray line = R"({"ok":true,"data":{"workspace":2,"focused":7,
        "workspaces":[{"index":1,"layout":"floating","windows":0,"active":false},
                      {"index":2,"layout":"grid","windows":3,"active":true}],
        "windows":[{"id":5,"title":"one","focused":false},{"id":7,"title":"Terminal","focused":true}],
        "outputs":[]}})";

    const SlateState state = parseState(line);
    QVERIFY(state.valid);
    QCOMPARE(state.workspace, 2);
    QCOMPARE(state.workspaces.size(), 2);
    QCOMPARE(state.workspaces[1], (WorkspaceInfo{.index = 2, .windows = 3, .active = true}));
    QCOMPARE(state.focusedTitle, QStringLiteral("Terminal"));
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

QTEST_APPLESS_MAIN(SlateStateTest)

#include "SlateStateTest.moc"
