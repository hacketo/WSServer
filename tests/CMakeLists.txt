project(tests)

add_executable(runUnitTests
        main_tests.cpp
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
        worker_test.cpp)
        #client_test.cpp)

target_link_libraries(runUnitTests libgtest libgmock WSServerlib)
