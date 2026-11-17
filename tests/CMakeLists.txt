# SPDX-License-Identifier: GPL-3.0-only

qt_add_executable(slate-state-test
    SlateStateTest.cpp
    ${PROJECT_SOURCE_DIR}/src/SlateState.cpp)
target_include_directories(slate-state-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(slate-state-test PRIVATE Qt6::Core Qt6::Test)
add_test(NAME slate-state COMMAND slate-state-test)

qt_add_executable(calendar-test
    CalendarTest.cpp
    ${PROJECT_SOURCE_DIR}/src/Calendar.cpp)
target_include_directories(calendar-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(calendar-test PRIVATE Qt6::Core Qt6::Test)
add_test(NAME calendar COMMAND calendar-test)

qt_add_executable(notification-store-test
    NotificationStoreTest.cpp
    ${PROJECT_SOURCE_DIR}/src/NotificationStore.cpp)
target_include_directories(notification-store-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(notification-store-test PRIVATE Qt6::Core Qt6::Qml Qt6::Test)
add_test(NAME notification-store COMMAND notification-store-test)

qt_add_executable(quick-settings-test
    QuickSettingsTest.cpp
    ${PROJECT_SOURCE_DIR}/src/QuickSettings.cpp)
target_include_directories(quick-settings-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(quick-settings-test PRIVATE Qt6::Core Qt6::DBus Qt6::Qml Qt6::Test)
add_test(NAME quick-settings COMMAND quick-settings-test)

qt_add_executable(app-catalog-test
    AppCatalogTest.cpp
    ${PROJECT_SOURCE_DIR}/src/AppCatalog.cpp)
target_include_directories(app-catalog-test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(app-catalog-test PRIVATE Qt6::Core Qt6::Qml Qt6::Test)
add_test(NAME app-catalog COMMAND app-catalog-test)
