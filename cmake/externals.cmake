# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)


include(externals/gtest)


# Search OpenSSL
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIRS})
  message(WARNING "Using OpenSSL ${OPENSSL_VERSION}")
endif ()


# Search Boost
find_package(Boost 1.54.0 COMPONENTS program_options thread date_time filesystem system REQUIRED)
if (Boost_FOUND)
  message(WARNING "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
  message(WARNING "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
  message(WARNING "Boost_LIBRARIES: ${Boost_LIBRARIES}")
  message(WARNING "Boost_VERSION: ${Boost_VERSION}")
else()
  #include(externals/boost)

endif ()
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
set(Boost_USE_STATIC_LIBS ON) # only find static libs
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)


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