cmake_minimum_required(VERSION 3.2)
project(Server)

message(WARNING "start running cmake...")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES src/main.cpp)


message(WARNING "sources files : ${SOURCE_FILES}")


# Search OpenSSL
find_package(OpenSSL REQUIRED)
if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(WARNING "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()


# Search Boost
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

if (NOT DEFINED     BOOST_ROOT        AND
        NOT DEFINED ENV{BOOST_ROOT}       AND
        NOT DEFINED     BOOST_INCLUDEDIR  AND
        NOT DEFINED ENV{BOOST_INCLUDEDIR} AND
        NOT DEFINED     BOOST_LIBRARYDIR  AND
        NOT DEFINED ENV{BOOST_LIBRARYDIR})
    if (APPLE)
        set(BOOST_ROOT "../../../boost/boost_1_54_0/mac")
    elseif (WIN32)
        set(BOOST_INCLUDEDIR "C:/local/boost_1_55_0")
        set(BOOST_LIBRARYDIR "C:/local/boost_1_55_0/lib32-msvc-10.0")
    endif()
endif()
if (APPLE OR WIN32)
    set(Boost_USE_STATIC_LIBS TRUE)
endif()

find_package(Boost 1.54.0 COMPONENTS thread date_time filesystem system REQUIRED)
if(Boost_FOUND)
    message(WARNING "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(WARNING "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(WARNING "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
else()

endif()

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


add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(tests)

add_executable(Server_run ${SOURCE_FILES} )
add_executable(Server_run2 src/main_state.cpp )

target_link_libraries(Server_run Server_src ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} pthread boost_timer rapidjson )
target_link_libraries(Server_run2 Server_src ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} pthread boost_timer rapidjson )

