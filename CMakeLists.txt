cmake_minimum_required(VERSION 3.16)
project(quick_backup_gtkmm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)

add_executable(quick-backup-gtkmm
    src/main.cpp
    src/MainWindow.cpp
    src/RsyncRunner.cpp
)

target_include_directories(quick-backup-gtkmm PRIVATE src)
target_link_libraries(quick-backup-gtkmm PRIVATE PkgConfig::GTKMM)
