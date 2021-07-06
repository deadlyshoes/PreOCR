set(CMAKE_AUTOMOC ON)

cmake_minimum_required(VERSION 3.5)

project(PreOCR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets)

if (Qt5_FOUND)
    add_executable(PreOCR_gui gui.h gui.cpp preocr.h preocr.cpp pbm.h pbm.cpp main_gui.cpp)
    target_link_libraries(PreOCR_gui Qt5::Widgets)
endif()

add_executable(PreOCR main.cpp pbm.h pbm.cpp preocr.h preocr.cpp)
