CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(cpu LANGUAGES C VERSION 0.0.1 DESCRIPTION "6502 emulator")

set(CMAKE_BINARY_DIR "./")
set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE DEBUG)
find_package(Curses REQUIRED)

file(COPY "./headers/cpu.h" DESTINATION "${CMAKE_BINARY_DIR}/headers/cpu.h")

include_directories(headers)
include_directories(${CURSES_INCLUDE_DIR})
file(GLOB CPU_SOURCES "src/cpu.c" "src/logger.c" "src/cartridge.c")
file(GLOB MONITOR_SOURCES "src/ui/[A-Za-z]*.c")

set(CMAKE_C_FLAGS_DEBUG "-g -O0 -std=c11 -Wall -Wextra -fsanitize=address -fsanitize=undefined -DDEBUG")
set(CMAKE_C_FLAGS "-std=c11")

if(WIN32) 
	add_definitions("")
endif(WIN32)

if(UNIX)
endif(UNIX)

add_library(cpu STATIC ${CPU_SOURCES})
add_executable(monitor ${MONITOR_SOURCES})

target_link_options(cpu PUBLIC "-D DEBUG")
target_link_options(monitor PUBLIC "-D DEBUG ")

set_target_properties(cpu PROPERTIES PUBLIC_HEADER "../headers/cpu.h")

target_link_libraries(monitor cpu)
target_link_libraries(monitor ${CURSES_LIBRARIES})
