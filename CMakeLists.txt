cmake_minimum_required(VERSION 3.15)

project(beacondbg)

if (MSVC)
else()
endif()

include_directories(include)

add_definitions(-D_UNICODE -DUNICODE)

set(SOURCES src/main.cpp)
add_executable(beacondbg ${SOURCES})