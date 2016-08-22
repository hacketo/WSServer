//
// Created by hacketo on 22/08/16.
//

#include <boost/asio/ip/tcp.hpp>

#include "gmock/gmock.h"
#include "gtest/gtest.h"


#define MOCK_SOCKET \
#include "mocks/socket_mock.h"\
#define SOCKET_CLASS MockSocket\


#undef USE_MODULES
#undef USE_SESSIONS

#include <client/client.h>


using ::testing::AtLeast;


TEST(client_test, init_test){

	MockSocket socket;

	ClientsManager::u_ptr cM = ClientsManager::u_ptr(new ClientsManager(new Manager));

	io_service io_service;
	Client::s_ptr c = cM->new_client(io_service);

	EXPECT_CALL(socket, remote_endpoint()).Times(AtLeast(1));

	c->start();


}

int main(int argc, char** argv) {
	// The following line must be executed to initialize Google Mock
	// (and Google Test) before running the tests.
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}