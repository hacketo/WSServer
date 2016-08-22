project(Server_src)

set(HEADER_FILES
    debug.h

    client/client.h
    client/manager/manager.h
    client/manager/statebased_manager.h

    db/dbhandler.h


    ext/modules/client_timer.h
    ext/modules/pub_sub.h
    ext/modules/base_module.h
    ext/sessions/sessionmanager.h
    ext/sessions/sessiondb.h

    protocol/constant.h
    protocol/frame.h
    protocol/http.h
    protocol/packet/packet.h
    protocol/opcode.h
    protocol/statuscode.h

    server/config.h
    server/errors.h
    server/server.h

    util/algo.h
    util/base64.h
    util/date.h
    util/exception.h
    util/genericvalue.h
    util/sha1.h
    util/string.h
    util/synchronizedpool.h
    util/timer.h
    util/worker.h
)

set(SOURCE_FILES
    debug.cpp

    client/client.cpp
    client/manager/manager.cpp
    client/manager/statebased_manager.cpp

    db/dbhandler.cpp

    ext/modules/client_timer.cpp
    ext/modules/pub_sub.cpp
    ext/modules/base_module.cpp
    ext/sessions/sessionmanager.cpp
    ext/sessions/sessiondb.cpp

    protocol/frame.cpp
    protocol/http.cpp
    protocol/packet/packet.cpp
    protocol/opcode.cpp

    server/config.cpp
    server/errors.cpp

    util/algo.cpp
    util/base64.cpp
    util/date.cpp
    util/sha1.cpp
    util/timer.cpp
    util/genericvalue.cpp
)

add_library(Server_src STATIC ${SOURCE_FILES} ${HEADER_FILES} )

add_library(red_src STATIC ${Server_src} impl/RED.cpp impl/RED.h)

add_library(chat_src STATIC ${Server_src} impl/Chat.cpp impl/Chat.h)
target_compile_definitions(chat_src PRIVATE USE_MODULES=1)
