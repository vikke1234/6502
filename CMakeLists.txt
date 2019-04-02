CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)
PROJECT(cpu)

include_directories(headers)

file(GLOB SOURCES "src/[a-z0-9]*.cpp")
file(GLOB TEST_SOURCES "tests/[a-z0-9]*.cpp")

add_definitions("-g")

add_executable(cpu ${SOURCES})
add_executable(tests ${TEST_SOURCES})
