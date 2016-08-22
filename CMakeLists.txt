cmake_minimum_required(VERSION 3.2)

project(Server)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -E")

#include(ExternalProject)
#include(External_boost)


# Search OpenSSL
find_package(OpenSSL REQUIRED)
if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(WARNING "Using OpenSSL ${OPENSSL_VERSION}")
endif()


# Search Boost
find_package(Boost 1.54.0 COMPONENTS thread date_time filesystem system REQUIRED)
if(Boost_FOUND)
    message(WARNING "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(WARNING "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
    message(WARNING "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(WARNING "Boost_VERSION: ${Boost_VERSION}")
    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})

    set(Boost_USE_STATIC_LIBS        ON) # only find static libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    OFF)
endif()


# Search SQLite3
find_package(SQLite3 REQUIRED)
if(SQLITE3_FOUND)
    message(WARNING "SQLITE3_INCLUDE_DIRS: ${SQLITE3_INCLUDE_DIRS}")
    message(WARNING "SQLITE3_LIBRARIES: ${SQLITE3_LIBRARIES}")
    include_directories(${SQLITE3_INCLUDE_DIRS})
endif()


# Search Curses
find_package(Curses QUIET)
if(CURSES_FOUND)
    message(WARNING "CURSES_INCLUDE_DIR: ${CURSES_INCLUDE_DIR}")
    message(WARNING "CURSES_LIBRARIES: ${CURSES_LIBRARIES}")
    message(WARNING "CURSES_HAVE_CURSES_H: ${CURSES_HAVE_CURSES_H}")
    message(WARNING "CURSES_HAVE_NCURSES_H: ${CURSES_HAVE_NCURSES_H}")
    include_directories(${CURSES_INCLUDE_DIR})

    add_executable(test_run src/test.cpp )
    target_link_libraries(test_run ${CURSES_LIBRARIES} curses panel menu)
endif()


add_subdirectory(lib)


add_executable(chat src/main.cpp )
add_executable(red src/main_state.cpp )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)



#link_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

add_subdirectory(src)

add_subdirectory(tests)

set(server_required_libraries ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} pthread boost_timer rapidjson sqlite3)

target_link_libraries(chat chat_src ${server_required_libraries} )

target_link_libraries(red red_src ${server_required_libraries} )
