cmake_minimum_required(VERSION 3.2)

project(Chat)

include_directories(../../include)

add_executable(Chat src/main.cpp )

add_library(Chat_lib STATIC src/Chat.cpp src/Chat.h)
target_compile_definitions(Chat_lib PRIVATE USE_MODULES=1)

target_link_libraries(Chat Chat_lib WSServerlib)