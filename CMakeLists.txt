cmake_minimum_required(VERSION 3.10)

project(Keylogger VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 COMPONENTS Widgets REQUIRED)

# --- CLI Keylogger ---
add_executable(keylogger
    src/keylogger.cpp
    src/app/application.cpp
    src/utils/common/common.cpp
    src/utils/keyboardEventHelper/keyboardEventHelper.cpp
    src/utils/cyrillicHelper/cyrillicHelper.cpp
    src/utils/specialKeysHelper/specialKeysHelper.cpp
    src/utils/windowTrackingHelper/windowTracking.cpp
    src/utils/mouseTrackerHelper/mouseTracker.cpp
)

target_link_libraries(keylogger PRIVATE X11 Xi)

# --- GUI Application ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(keylogger-gui
    src/ui/main_ui.cpp
    src/ui/windows/MainWindow.cpp
    src/ui/components/BrutalistButton.cpp
    src/ui/components/AnimatedBorder.cpp
    src/ui/components/KeyLogBlock.cpp
    src/ui/widgets/NotificationWidget.cpp
    src/ui/utils/WindowColorManager/WindowColorManager.cpp
    src/ui/pages/LogsPageWidget/LogsPageWidget.cpp
    src/ui/pages/PlaceholderPageWidget/PlaceholderPageWidget.cpp
    src/ui/pages/ClicksPageWidget/ClicksPageWidget.cpp
    src/app/application.cpp
    src/utils/common/common.cpp
    src/utils/keyboardEventHelper/keyboardEventHelper.cpp
    src/utils/cyrillicHelper/cyrillicHelper.cpp
    src/utils/specialKeysHelper/specialKeysHelper.cpp
    src/utils/windowTrackingHelper/windowTracking.cpp
    src/utils/mouseTrackerHelper/mouseTracker.cpp
)

target_link_libraries(keylogger-gui PRIVATE Qt6::Widgets)

find_package(X11 REQUIRED)
find_program(XCLIP_EXECUTABLE NAMES xclip)
find_program(SCROT_EXECUTABLE NAMES scrot)

if(X11_FOUND)
    target_include_directories(keylogger PRIVATE ${X11_INCLUDE_DIR})
    target_include_directories(keylogger-gui PRIVATE ${X11_INCLUDE_DIR})

    target_link_libraries(keylogger PRIVATE 
        ${X11_LIBRARIES}
        ${X11_Xinput_LIB}
    )

    target_link_libraries(keylogger-gui PRIVATE 
        ${X11_LIBRARIES}
        ${X11_Xinput_LIB}
    )
endif()

target_include_directories(keylogger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})