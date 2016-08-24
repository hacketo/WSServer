//
// Created by hacketo on 24/08/16.
//

#ifndef WSSERVERLIB_CLIENT_MOCK_H
#define WSSERVERLIB_CLIENT_MOCK_H

#include "gmock/gmock.h"
#include <mocks/socket_mock.h>

#ifdef USE_MODULES
#include "ext/modules/base_module.h"
using namespace ext::modules;
#endif


#include <client/client.h>

using namespace protocol;



class MockClient : public Client {
public:

	MockClient(MockSocket& socket) : Client(socket) {}

	MOCK_METHOD0(start, void());
	MOCK_METHOD1(send, void(std::string& message));
	MOCK_METHOD1(send, void(frame::FrameInterface* holder));
	MOCK_METHOD1(send, void(frame::Frame* frame));
	MOCK_METHOD0(isAlive, bool());

#ifdef USE_MODULES
	MOCK_METHOD1(registerModule, bool(base_module* module));
	MOCK_METHOD1(unregisterModule, bool(base_module* module));
	MOCK_METHOD1(hasModuleRegistered, bool(uint64_t moduleId));
	MOCK_METHOD1(getModuleController, ModuleClientController*(uint64_t moduleId));
#endif

	MOCK_METHOD0(get_ip, std::string());
	MOCK_METHOD0(get_id, uint32_t());

	MOCK_METHOD0(joinThreads, void());
	MOCK_METHOD0(closeSocket, void());

	MOCK_METHOD2(get_http_header, void(http::handshake* handshake, errors::error_code& error));
	MOCK_METHOD2(send_handshake, void(const char* websocket_key, errors::error_code& error));
	MOCK_METHOD2(recv_sync, size_t(char *data, boost::system::error_code &error));

	MOCK_METHOD3(send_sync, void(unsigned char *data, size_t size, boost::system::error_code &error));
};

#endif //WSSERVERLIB_CLIENT_MOCK_H
