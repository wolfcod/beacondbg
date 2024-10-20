cmake_minimum_required(VERSION 3.15)

project(beaconcli)

if (MSVC)
else()
endif()

include_directories(../include)

add_definitions(-D_UNICODE -DUNICODE)

set(SOURCES cli.cpp)
add_library(beaconcli STATIC ${SOURCES})