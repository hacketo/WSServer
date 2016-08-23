cmake_minimum_required(VERSION 3.2)

project(Chat)

include_directories(../../include)

add_executable(Chat src/main.cpp )

add_library(Chat_lib STATIC src/Chat.cpp src/Chat.h)
target_compile_definitions(Chat_lib PUBLIC USE_MODULES=1 USE_SESSIONS=0 WS_DEBUG=false)

target_link_libraries(Chat Chat_lib WSServerlib)