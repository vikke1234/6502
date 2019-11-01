CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(emulator LANGUAGES C VERSION 0.0.1 DESCRIPTION "6502 emulator")

set(CMAKE_BINARY_DIR "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE DEBUG)

find_package(SDL2 REQUIRED)
find_package(Git REQUIRED)

include_directories(headers)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB CPU_SOURCES "src/cpu.c" "src/logger.c" "src/cartridge.c")
file(GLOB EMULATOR_SOURCES "src/main.c" "src/ui/gui.c")

add_library(cpu STATIC ${CPU_SOURCES})
add_executable(${PROJECT_NAME} ${EMULATOR_SOURCES})

set_property(TARGET cpu PROPERTY C_STANDARD 11)
set_target_properties(cpu PROPERTIES PUBLIC_HEADER "../headers/cpu.h")

set_property(TARGET emulator PROPERTY C_STANDARD 11)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git" AND NOT SDL2_FOUND)
  if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}")
    endif()
  endif()
endif()

if(WIN32) 
	add_definitions("-D _CRT_SECURE_NO_WARNINGS")
  #figure out how to do package management for this
endif(WIN32)

if(UNIX)
	set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -fsanitize=address -fsanitize=undefined")
  #set_property(TARGET cpu PROPERTY STATIC_LIBRARY_OPTIONS "-D DEBUG")
	target_link_options(emulator PUBLIC "-D DEBUG")
endif(UNIX)

# SDL2::SDL2 is some odd thing that arch does for some unknown reason https://discourse.libsdl.org/t/arch-linux-cmake-find-package-sdl2-required-passes-but-doesnt-find-anything/24226/2
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 cpu )
