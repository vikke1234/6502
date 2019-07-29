CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(cpu VERSION 0.0.1 DESCRIPTION "6502 emulator")

set(CMAKE_BINARY_DIR "./")
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Curses REQUIRED)

file(COPY "./headers/cpu.h" DESTINATION "${CMAKE_BINARY_DIR}/headers/cpu.h")

include_directories(headers)
include_directories(${CURSES_INCLUDE_DIR})
file(GLOB CPU_SOURCES "src/cpu.c")
file(GLOB MONITOR_SOURCES "src/gui.c")

if(WIN32) 
	add_definitions("")
endif(WIN32)

if(UNIX)
	add_definitions("-g -std=c11 -Wall")
endif(UNIX)

add_library(cpu STATIC ${CPU_SOURCES})
add_executable(monitor ${MONITOR_SOURCES})

set_target_properties(cpu PROPERTIES PUBLIC_HEADER "../headers/cpu.h")

target_link_libraries(monitor cpu)
target_link_libraries(monitor ${CURSES_LIBRARIES})
