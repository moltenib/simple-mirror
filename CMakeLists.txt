cmake_minimum_required(VERSION 3.16)
project(quick_backup_qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(quick-backup-qt
    src/main.cpp
    src/MainWindow.cpp
    src/RsyncRunner.cpp
    src/DirectoryChooserWidget.cpp
)

target_include_directories(quick-backup-qt PRIVATE src)
target_link_libraries(quick-backup-qt PRIVATE Qt6::Widgets)
