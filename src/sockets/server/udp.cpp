//
// Created by hacketo on 03/09/16.
//

#include <boost/lexical_cast.hpp>
#include "sockets/server/udp.h"
#include "client/client.h"

namespace sockets {
namespace server {

	Udp::Udp(ClientManager *clientManager, short port) :
			ServerSocket(clientManager), m_endpoint(asio::ip::address_v4::any(), port),
			m_socket(m_ios, m_endpoint.protocol()) {

	}

	void Udp::start(errors::error_code& ec) {
		boost::system::error_code e;
		m_socket.bind(m_endpoint, e);

		if (e) {
			ec = errors::error_code("Failed to bind the sockets.", e);
		}
		else {
			std::string ip = boost::lexical_cast<std::string>(m_endpoint);
			std::cout << "listening on : " << ip << std::endl;
			ServerSocket::start(ec);
		}

	}

	void Udp::close() {
		m_socket.close();
		ServerSocket::close();
	}

	void Udp::loop() {
		if (!m_closed.load()) {
			std::unique_ptr<boost::asio::streambuf> buffer(new boost::asio::streambuf);
			asio::streambuf::mutable_buffers_type buf = buffer.get()->prepare(400);

			asio::ip::udp::endpoint sender_endpoint;

			m_socket.async_receive_from(buf, sender_endpoint,
				[this, sender_endpoint](boost::system::error_code ec, std::size_t bytes_recvd) {
					if (!ec && bytes_recvd > 0) {

						sockets::Udp *s = new sockets::Udp(&m_socket, sender_endpoint);

						m_clientManager->handle_new_socket(s);
					} else {
						loop();
					}
			});
		}
		else{

		}
	}

}
}