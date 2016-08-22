cmake_minimum_required(VERSION 3.2)

project(Red)

include_directories(../../include)

add_executable(Red src/main.cpp)

add_library(Red_lib STATIC src/RED.cpp src/RED.h)

target_link_libraries(Red Red_lib WSServerlib)