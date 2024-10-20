cmake_minimum_required(VERSION 3.15)

project(coffloader)

if (MSVC)
else()
endif()

include_directories(../include)

add_definitions(-D_UNICODE -DUNICODE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES loader.c)
add_library(coffloader STATIC ${SOURCES})