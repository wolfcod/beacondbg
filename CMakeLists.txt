cmake_minimum_required(VERSION 3.15)

project(beacondbg)

if (MSVC)
else()
endif()

include_directories(include)

add_definitions(-D_UNICODE -DUNICODE)
add_subdirectory(cli)

set(SOURCES src/main.cpp)

add_executable(beacondbg ${SOURCES})

add_dependencies(beacondbg beaconcli)
target_link_libraries(beacondbg beaconcli)
