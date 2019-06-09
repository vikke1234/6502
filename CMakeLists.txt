CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(cpu VERSION 0.0.1 DESCRIPTION "6502 emulator")

include_directories(headers)

file(GLOB SOURCES "src/cpu.c")

add_definitions("-g -D DEBUG -std=c11")
add_library(cpu STATIC ${SOURCES})
set_target_properties(cpu PROPERTIES PUBLIC_HEADER "../headers/cpu.h")
