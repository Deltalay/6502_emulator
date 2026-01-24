cmake_minimum_required(VERSION 3.25)

project(6502_emulator LANGUAGES C)

file(GLOB SOURCES "src/*.c")
add_executable(emulator ${SOURCES})
