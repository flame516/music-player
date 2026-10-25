cmake_minimum_required(VERSION 3.16)
project(MusicPlayerQt6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Sql)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/playerpanel.cpp
    src/playerpanel.h
    src/logindialog.cpp
    src/logindialog.h
    src/registerdialog.cpp
    src/registerdialog.h
    src/databasemanager.cpp
    src/databasemanager.h
    src/heartbutton.cpp
    src/heartbutton.h
)

qt_standard_project_setup()

qt_add_executable(MusicPlayerQt6
    ${PROJECT_SOURCES}
)

target_link_libraries(MusicPlayerQt6 PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Sql
)

if (WIN32)
    set_target_properties(MusicPlayerQt6 PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()


