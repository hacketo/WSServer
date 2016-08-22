cmake_minimum_required(VERSION 3.2)

project(Red)

include_directories(../../include)

add_executable(Red src/main.cpp src/RED.cpp src/RED.h)

target_link_libraries(Red WSServerlib)