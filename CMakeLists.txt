cmake_minimum_required(VERSION 3.6.1)

project(WSServerlib)

if(NOT DEFINED USE_MODULES)
  set(USE_MODULES 1)
endif()
if(NOT DEFINED USE_SESSIONS)
  set(USE_SESSIONS 1)
endif()
if(NOT DEFINED WS_DEBUG)
  set(WS_DEBUG 1)
endif()
if(NOT DEFINED PREPROCESS_ONLY)
  set(PREPROCESS_ONLY 0)
endif()


if (CMAKE_BUILD_TYPE EQUAL "Debug")
  set(ERROR_HANDLER 1)
endif()


set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


if (PREPROCESS_ONLY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -E")
endif ()
if (ERROR_HANDLER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -rdynamic")
endif ()


include(externals)
add_subdirectory(libs/rapidjson)

set(M_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GOOGLETEST_H} ${GOOGLEMOCK_H})

set(WSServerlib_MINCLUDE_PATH ${M_INCLUDE_PATH} PARENT_SCOPE)


include_directories(${M_INCLUDE_PATH})

set(server_required_libraries ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} pthread boost_timer rapidjson sqlite3 libgtest libgmock)


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
  include/mocks/client_mock.h
  include/mocks/socket_mock.h
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
  src/util/genericvalue.cpp
  src/util/worker.cpp)

set(MODULES_FILES src/ext/modules/client_timer.cpp
        src/ext/modules/pub_sub.cpp
        src/ext/modules/base_module.cpp)

set(SESSIONS_FILES src/ext/sessions/sessionmanager.cpp)

if (USE_MODULES)
  list(APPEND SOURCE_FILES ${MODULES_FILES})
endif ()
if (USE_SESSIONS)
  list(APPEND SOURCE_FILES ${SESSIONS_FILES})
endif ()

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
if (DEFINED WS_DEBUG)
  message(WARNING "WS_DEBUG=${WS_DEBUG}")
  target_compile_definitions(${PROJECT_NAME} PUBLIC WS_DEBUG=${WS_DEBUG})
endif ()

target_link_libraries(${PROJECT_NAME} ${server_required_libraries})

# Install rules
install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include/${PROJECT_NAME})

add_subdirectory(tests)
add_subdirectory(impl/chat)
add_subdirectory(impl/red)