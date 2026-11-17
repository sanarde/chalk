# SPDX-License-Identifier: GPL-3.0-only


qt_add_executable(chalk main.cpp)
target_compile_definitions(chalk PRIVATE CHALK_VERSION="${PROJECT_VERSION}")

qt_add_qml_module(chalk
    URI Sanar.Chalk
    VERSION 1.0
    QML_FILES
        qml/Bar.qml
        qml/PopupWindow.qml
        qml/CalendarPopup.qml
        qml/QuickTile.qml
        qml/QuickSettingsPopup.qml
        qml/StatusPill.qml
        qml/NotificationBanner.qml
        qml/WorkspaceChip.qml
    SOURCES
        NotificationStore.h
        NotificationStore.cpp
        NotificationServer.h
        NotificationServer.cpp
        QuickSettings.h
        QuickSettings.cpp
        Calendar.h
        Calendar.cpp
        SlateState.h
        SlateState.cpp
        SlateClient.h
        SlateClient.cpp
        PanelProperties.h
        PanelProperties.cpp
        PanelShell.h
        PanelShell.cpp
)

target_link_libraries(chalk PRIVATE
    Qt6::Core Qt6::Gui Qt6::Quick Qt6::Network Qt6::DBus
    SanarStyle::sanar-style
    PkgConfig::XCB)

install(TARGETS chalk RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
