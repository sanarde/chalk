# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.21)
project(chalk VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Quick Network DBus)
find_package(Qt6 6.8 QUIET COMPONENTS Test)
find_package(SanarStyle REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb xcb-shape)
qt_standard_project_setup(REQUIRES 6.8)

include(GNUInstallDirs)

add_subdirectory(src)
add_subdirectory(layershell)

if(Qt6Test_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()
