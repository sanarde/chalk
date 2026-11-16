# SPDX-License-Identifier: GPL-3.0-only

set_source_files_properties(qml/Tokens.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_executable(chalk main.cpp)
target_compile_definitions(chalk PRIVATE CHALK_VERSION="${PROJECT_VERSION}")

qt_add_qml_module(chalk
    URI Sanar.Chalk
    VERSION 1.0
    QML_FILES
        qml/Bar.qml
        qml/BarButton.qml
        qml/CalendarPopup.qml
        qml/Tokens.qml
        qml/WorkspaceChip.qml
    SOURCES
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
    Qt6::Core Qt6::Gui Qt6::Quick Qt6::Network
    SanarStyle::sanar-style
    PkgConfig::XCB)

install(TARGETS chalk RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
