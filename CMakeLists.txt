cmake_minimum_required(VERSION 3.25)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
project(6502_emulator LANGUAGES C)

file(GLOB SOURCES "src/*.c")
add_executable(emulator ${SOURCES})
