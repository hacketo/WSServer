cmake_minimum_required(VERSION 3.2)

project(WSServerlib)

set(USE_MODULES 1)
set(USE_SESSIONS 1)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -E")

# Search OpenSSL
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIRS})
  message(WARNING "Using OpenSSL ${OPENSSL_VERSION}")
endif ()


# Search Boost
find_package(Boost 1.54.0 COMPONENTS thread date_time filesystem system REQUIRED)
if (Boost_FOUND)
  message(WARNING "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
  message(WARNING "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
  message(WARNING "Boost_LIBRARIES: ${Boost_LIBRARIES}")
  message(WARNING "Boost_VERSION: ${Boost_VERSION}")
  link_directories(${Boost_LIBRARY_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})

  set(Boost_USE_STATIC_LIBS ON) # only find static libs
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
endif ()


# Search SQLite3
find_package(SQLite3 REQUIRED)
if (SQLITE3_FOUND)
  message(WARNING "SQLITE3_INCLUDE_DIRS: ${SQLITE3_INCLUDE_DIRS}")
  message(WARNING "SQLITE3_LIBRARIES: ${SQLITE3_LIBRARIES}")
  include_directories(${SQLITE3_INCLUDE_DIRS})
endif ()


# Search Curses
find_package(Curses QUIET)
if (CURSES_FOUND)
  message(WARNING "CURSES_INCLUDE_DIR: ${CURSES_INCLUDE_DIR}")
  message(WARNING "CURSES_LIBRARIES: ${CURSES_LIBRARIES}")
  message(WARNING "CURSES_HAVE_CURSES_H: ${CURSES_HAVE_CURSES_H}")
  message(WARNING "CURSES_HAVE_NCURSES_H: ${CURSES_HAVE_NCURSES_H}")
  include_directories(${CURSES_INCLUDE_DIR})

  add_executable(test_run src/test.cpp)
  target_link_libraries(test_run ${CURSES_LIBRARIES} curses panel menu)
endif ()

add_subdirectory(libs)

set(server_required_libraries ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} pthread boost_timer rapidjson sqlite3)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/rapidjson
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADER_FILES
  include/debug.h
  include/client/client.h
  include/client/manager.h
  include/client/statebased_manager.h
  include/db/dbhandler.h
  include/ext/modules/client_timer.h
  include/ext/modules/pub_sub.h
  include/ext/modules/base_module.h
  include/ext/sessions/sessionmanager.h
  include/protocol/constant.h
  include/protocol/frame.h
  include/protocol/http.h
  include/protocol/packet.h
  include/protocol/opcode.h
  include/protocol/statuscode.h
  include/server/config.h
  include/server/errors.h
  include/server/server.h
  include/util/algo.h
  include/util/base64.h
  include/util/date.h
  include/util/exception.h
  include/util/genericvalue.h
  include/util/sha1.h
  include/util/safedeque.h
  include/util/timer.h
  include/util/worker.h)


set(SOURCE_FILES
  src/debug.cpp
  src/client/client.cpp
  src/client/manager.cpp
  src/client/statebased_manager.cpp
  src/db/dbhandler.cpp
  src/ext/modules/client_timer.cpp
  src/ext/modules/pub_sub.cpp
  src/ext/modules/base_module.cpp
  src/ext/sessions/sessionmanager.cpp
  src/protocol/frame.cpp
  src/protocol/http.cpp
  src/protocol/packet.cpp
  src/protocol/opcode.cpp
  src/server/config.cpp
  src/server/errors.cpp
  src/util/base64.cpp
  src/util/date.cpp
  src/util/sha1.cpp
  src/util/timer.cpp
  src/util/genericvalue.cpp)


set(SERVER_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_library(${PROJECT_NAME} SHARED ${SERVER_FILES})


if (USE_MODULES)
  message(WARNING "USE_MODULES=1")
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_MODULES=1)
endif ()
if (USE_SESSIONS)
  message(WARNING "USE_SESSIONS=1")
  target_compile_definitions(${PROJECT_NAME} PUBLIC USE_SESSIONS=1)
endif ()


target_link_libraries(${PROJECT_NAME} ${server_required_libraries})


# Install rules
install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include/${PROJECT_NAME})


add_subdirectory(tests)
add_subdirectory(impl/chat)
add_subdirectory(impl/red)