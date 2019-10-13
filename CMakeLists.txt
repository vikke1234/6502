CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(cpu LANGUAGES C VERSION 0.0.1 DESCRIPTION "6502 emulator")

set(CMAKE_BINARY_DIR "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE DEBUG)

file(COPY "headers/cpu.h" DESTINATION "${CMAKE_BINARY_DIR}headers/")

include_directories(headers)

file(GLOB CPU_SOURCES "src/cpu.c" "src/logger.c" "src/cartridge.c")
file(GLOB MONITOR_SOURCES "src/ui/[A-Za-z]*.c")

	
if(WIN32) 
	add_definitions("-D _CRT_SECURE_NO_WARNINGS")
endif(WIN32)

if(UNIX)
	set(CMAKE_C_FLAGS_DEBUG "-g -O0-Wall -Wextra -fsanitize=address -fsanitize=undefined -DDEBUG")
	target_link_options(cpu PUBLIC "-D DEBUG")
	target_link_options(monitor PUBLIC "-D DEBUG ")
endif(UNIX)

add_library(cpu STATIC ${CPU_SOURCES})
add_executable(monitor ${MONITOR_SOURCES})



set_target_properties(cpu PROPERTIES PUBLIC_HEADER "../headers/cpu.h")
set_property(TARGET monitor PROPERTY C_STANDARD 11)

target_link_libraries(monitor cpu)
set_property(TARGET monitor PROPERTY C_STANDARD 11)


target_link_libraries(monitor)
