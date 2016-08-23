project(libs)

add_library(rapidjson STATIC
  rapidjson/allocators.h
  rapidjson/document.h
  rapidjson/encodedstream.h
  rapidjson/encodings.h
  rapidjson/filereadstream.h
  rapidjson/filewritestream.h
  rapidjson/fwd.h
  rapidjson/istreamwrapper.h
  rapidjson/memorybuffer.h
  rapidjson/memorystream.h
  rapidjson/ostreamwrapper.h
  rapidjson/pointer.h
  rapidjson/prettywriter.h
  rapidjson/rapidjson.h
  rapidjson/reader.h
  rapidjson/schema.h
  rapidjson/stream.h
  rapidjson/stringbuffer.h
  rapidjson/writer.h
  rapidjson/error/en.h
  rapidjson/error/error.h
  rapidjson/internal/biginteger.h
  rapidjson/internal/diyfp.h
  rapidjson/internal/dtoa.h
  rapidjson/internal/ieee754.h
  rapidjson/internal/itoa.h
  rapidjson/internal/meta.h
  rapidjson/internal/pow10.h
  rapidjson/internal/regex.h
  rapidjson/internal/stack.h
  rapidjson/internal/strfunc.h
  rapidjson/internal/strtod.h
  rapidjson/internal/swap.h
  rapidjson/msinttypes/inttypes.h
  rapidjson/msinttypes/stdint.h
)

set_target_properties(rapidjson PROPERTIES LINKER_LANGUAGE CXX)


# Download and install GoogleTest
ExternalProject_Add( gtest
        URL https://github.com/google/googletest/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        # Disable install step
        INSTALL_COMMAND ""
        )

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/googletest/include"
        "${source_dir}/googlemock/include")