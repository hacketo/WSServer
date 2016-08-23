//
// Created by hacketo on 22/08/16.
//

#include <boost/asio/ip/tcp.hpp>

#include "gmock/gmock.h"
#include "gtest/gtest.h"


#define MOCK_SOCKET
#define SOCKET_CLASS_H "mocks/socket_mock.h"
#define SOCKET_CLASS MockSocket


#define USE_MODULES
#undef USE_SESSIONS

#include <client/client.h>


using ::testing::AtLeast;


class ClientFixture : public ::testing::Test {
protected:
	virtual void TearDown(){

	}
	virtual void SetUp(){

	}
public:
	ClientFixture() : Test(){
		cM = ClientsManager::u_ptr(new ClientsManager(new Manager));
	}

	virtual ~ClientFixture(){}

	ClientsManager::u_ptr cM;
	MockSocket socket;
};


TEST_F(ClientFixture, client_init_test){
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	errors::init_handler();
	RecordProperty("ClientsManager ALive", cM->isAlive());
	cM->init();

	io_service io_service;
	Client::s_ptr c = cM->new_client(io_service);

	EXPECT_CALL(socket, remote_endpoint()).Times(AtLeast(1));

	c->start();


}

int main(int argc, char** argv) {
	// The following line must be executed to initialize Google Mock
	// (and Google Test) before running the tests.
	::testing::GTEST_FLAG(throw_on_failure) = true;
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}