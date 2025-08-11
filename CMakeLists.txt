cmake_minimum_required(VERSION 3.10)

project(Keylogger VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(keylogger
    src/keylogger.cpp
    src/utils/cyrillicHelper/cyrillicHelper.cpp
    src/utils/keyPressHelper/keyPressHelper.cpp
    src/utils/specialKeysHelper/specialKeysHelper.cpp
)

find_package(X11 REQUIRED)


if(X11_FOUND)
    target_include_directories(keylogger PRIVATE ${X11_INCLUDE_DIR})

    target_link_libraries(keylogger PRIVATE 
        ${X11_LIBRARIES}
        ${X11_Xinput_LIB}
    )
endif()

target_include_directories(keylogger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})