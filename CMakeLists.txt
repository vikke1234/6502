CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(cpu LANGUAGES C VERSION 0.0.1 DESCRIPTION "6502 emulator")

set(CMAKE_BINARY_DIR "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE DEBUG)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
file(COPY "headers/cpu.h" DESTINATION "${CMAKE_BINARY_DIR}headers/")

include_directories(headers)

file(GLOB CPU_SOURCES "src/cpu.c" "src/logger.c" "src/cartridge.c")
file(GLOB EMULATOR_SOURCES "src/main.c")

add_library(cpu STATIC ${CPU_SOURCES})
add_executable(emulator ${EMULATOR_SOURCES})

if(WIN32) 
	add_definitions("-D _CRT_SECURE_NO_WARNINGS")
endif(WIN32)

if(UNIX)
	set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined")
  #set_property(TARGET cpu PROPERTY STATIC_LIBRARY_OPTIONS "-D DEBUG")
	target_link_options(emulator PUBLIC "-D DEBUG")
endif(UNIX)

set_target_properties(cpu PROPERTIES PUBLIC_HEADER "../headers/cpu.h")
set_property(TARGET cpu PROPERTY C_STANDARD 11)

target_link_libraries(emulator cpu)
set_property(TARGET emulator PROPERTY C_STANDARD 11)


target_link_libraries(emulator)
