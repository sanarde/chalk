# SPDX-License-Identifier: GPL-3.0-only

qt_add_executable(slate-state-test
    SlateStateTest.cpp
    ${PROJECT_SOURCE_DIR}/src/SlateState.cpp)
target_include_directories(slate-state-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(slate-state-test PRIVATE Qt6::Core Qt6::Test)
add_test(NAME slate-state COMMAND slate-state-test)
