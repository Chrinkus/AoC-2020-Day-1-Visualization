cmake_minimum_required(VERSION 3.16)

project(aoc-20-01-vis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wextra -Wall -Wpedantic)
endif()

if(APPLE)
    # This is where a CMake installation of FLTK puts stuff on a Mac
    set(FLTK_DIR "/usr/local/FLTK/.framework/Resources/CMake/")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa")
endif()

find_package(FLTK REQUIRED NO_MODULE)
include_directories(${FLTK_INCLUDE_DIRS})

add_executable(2001v main.cpp)
target_link_libraries(2001v fltk)
