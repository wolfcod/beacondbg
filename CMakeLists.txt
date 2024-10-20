cmake_minimum_required(VERSION 3.15)

project(beacondbg)

if (MSVC)
else()
endif()

include_directories(include)

add_definitions(-D_UNICODE -DUNICODE)
add_subdirectory(cli)
add_subdirectory(coff)

set(SOURCES src/main.cpp)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(beacondbg ${SOURCES})

add_dependencies(beacondbg beaconcli coffloader)
target_link_libraries(beacondbg beaconcli coffloader)
