# SPDX-License-Identifier: GPL-3.0-only

find_package(Qt6 6.8 REQUIRED COMPONENTS Gui GuiPrivate WaylandClient WaylandClientPrivate)
pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)

add_library(chalk-layer-shell MODULE
    LayerShell.h
    LayerShell.cpp
    LayerSurface.h
    LayerSurface.cpp
    Plugin.h
    Plugin.cpp
    ${PROJECT_SOURCE_DIR}/src/PanelProperties.h
    ${PROJECT_SOURCE_DIR}/src/PanelProperties.cpp)

qt6_generate_wayland_protocol_client_sources(chalk-layer-shell
    PRIVATE_CODE
    FILES
        ${PROJECT_SOURCE_DIR}/protocols/xdg-shell.xml
        ${PROJECT_SOURCE_DIR}/protocols/wlr-layer-shell-unstable-v1.xml)

set_target_properties(chalk-layer-shell PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/wayland-shell-integration)

target_include_directories(chalk-layer-shell PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(chalk-layer-shell PRIVATE
    Qt6::Core Qt6::Gui Qt6::GuiPrivate Qt6::WaylandClient Qt6::WaylandClientPrivate
    PkgConfig::WAYLAND_CLIENT)

install(TARGETS chalk-layer-shell LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt6/plugins/wayland-shell-integration)
