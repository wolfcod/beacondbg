cmake_minimum_required(VERSION 3.15)

project(beaconcli)

if (MSVC)
else()
endif()

include_directories(../include)

add_definitions(-D_UNICODE -DUNICODE)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES cli.cpp clicmd.cpp breakpoints.cpp quit.cpp load.cpp)
add_library(beaconcli STATIC ${SOURCES})