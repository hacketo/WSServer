include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runUnitTests
        stringdata_test.cpp
        arraydata_test.cpp
        objectdata_test.cpp
        intdata_test.cpp
        floatdata_test.cpp
        booldata_test.cpp
        nulldata_test.cpp
        jsonparse_test.cpp
        modules_test.cpp)


target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests Server_src ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} pthread boost_timer rapidjson)