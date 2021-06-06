set(CMAKE_AUTOMOC ON)

cmake_minimum_required(VERSION 3.5)

project(PreOCR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(PreOCR gui.h gui.cpp ppm.h ppm.cpp main.cpp)

target_link_libraries(PreOCR Qt5::Widgets)
