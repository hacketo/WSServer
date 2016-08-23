set(boost_version 1.61.0)
set(boost_url "http://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.gz/download")
set(boost_md5 "015ae4afa6f3e597232bfe1dab949ace")


if("${CMAKE_CURRENT_BINARY_DIR}" MATCHES " ")
  message(FATAL_ERROR "cannot use boost bootstrap with a space in the name of the build dir")
endif()

if(NOT DEFINED use_bat)
  if(WIN32)
    set(use_bat 1)
  else()
    set(use_bat 0)
  endif()
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(am 64)
else()
  set(am 32)
endif()

set(boost_with_args
        --with-date_time
        --with-filesystem
        --with-iostreams
        --with-program_options
        --with-system
        --with-thread
        --with-chrono
        )

if(use_bat)
  if(MSVC90)
    set(_toolset "msvc-9.0")
  elseif(MSVC10)
    set(_toolset "msvc-10.0")
  elseif(MSVC11)
    set(_toolset "msvc-11.0")
  endif()

  list(APPEND boost_with_args
          "--layout=tagged" "toolset=${_toolset}")

  set(boost_cmds
          CONFIGURE_COMMAND bootstrap.bat
          BUILD_COMMAND b2 address-model=${am} ${boost_with_args}
          INSTALL_COMMAND b2 address-model=${am} ${boost_with_args}
          --prefix=<INSTALL_DIR> install
          )
else()
  set(boost_cmds
          CONFIGURE_COMMAND ./bootstrap.sh --prefix=<INSTALL_DIR>
          BUILD_COMMAND ./b2 address-model=${am} ${boost_with_args}
          INSTALL_COMMAND ./b2 address-model=${am} ${boost_with_args}
          install
          )
endif()

ExternalProject_Add(boost
        DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/tmp"
        URL ${boost_url}
        URL_MD5 ${boost_md5}
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/boost"
        INSTALL_DIR "${CMAKE_BINARY_DIR}/INSTALL"
        ${boost_cmds}
        BUILD_IN_SOURCE 1
        )

ExternalProject_Get_Property(boost install_dir)
set(BOOST_ROOT "${install_dir}" CACHE INTERNAL "")

set(Boost_LIBRARY_DIR ${BOOST_ROOT}/lib )
set(Boost_INCLUDE_DIR ${BOOST_ROOT}/include )
set(Boost_NO_SYSTEM_PATHS ON )

message(WARNING "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(WARNING "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(WARNING "Boost_VERSION: ${Boost_VERSION}")
