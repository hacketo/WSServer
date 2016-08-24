//
// Created by hacketo on 22/08/16.
//

#ifndef WSSERVERLIB_SOCKET_MOCK_H
#define WSSERVERLIB_SOCKET_MOCK_H

#include <boost/asio/ip/tcp.hpp>
#include "gmock/gmock.h"

using namespace boost::asio;

class SocketInterface {

public:
	SocketInterface(){}
	SocketInterface(io_service& io_service){}
	virtual ~SocketInterface(){}

	virtual ip::tcp::endpoint remote_endpoint() = 0;

	virtual void close() = 0;

	virtual size_t read_some(const mutable_buffers_1& buffers,
							 boost::system::error_code& ec) = 0;

	virtual void write_some(const const_buffers_1& buffers,
							boost::system::error_code& ec) = 0;

	virtual void async_read_some (const mutable_buffers_1& buffers,
					 void (boost::system::error_code, std::size_t)) = 0;

};

class MockSocket : public SocketInterface {
public:

	MockSocket() : SocketInterface() {}
	MockSocket(io_service& io_service) : SocketInterface(io_service) {}

	MOCK_METHOD0(close, void());
	MOCK_METHOD0(remote_endpoint, ip::tcp::endpoint());

	MOCK_METHOD2(read_some, size_t(const mutable_buffers_1& buffers, boost::system::error_code& ec));
	MOCK_METHOD2(write_some, void(const const_buffers_1& buffers, boost::system::error_code& ec));
	MOCK_METHOD2(async_read_some, void(const mutable_buffers_1& buffers,void (boost::system::error_code, std::size_t)));
};


#endif //WSSERVERLIB_SOCKET_MOCK_H
