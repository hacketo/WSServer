
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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
