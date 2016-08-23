project(tests)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ../include)

add_executable(runUnitTests
        stringdata_test.cpp
        arraydata_test.cpp
        objectdata_test.cpp
        intdata_test.cpp
        floatdata_test.cpp
        booldata_test.cpp
        nulldata_test.cpp
        jsonparse_test.cpp
        modules_test.cpp
        time_test.cpp
        safedeque_test.cpp
        client_test.cpp
        dumytest.cpp)

target_link_libraries(runUnitTests libgtest libgmock gtest_main WSServerlib)
