cmake_minimum_required(VERSION 3.2)

project(Chat)

include_directories(../../include)

add_executable(Chat src/main.cpp src/Chat.cpp src/Chat.h)
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_MODULES=1)

target_link_libraries(Chat WSServerlib)