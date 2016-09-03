//
// Created by hacketo on 03/09/16.
//

#include <boost/lexical_cast.hpp>
#include "sockets/udp.h"

namespace sockets {

	Udp::Udp(asio::ip::udp::socket* socket, asio::ip::udp::endpoint ep) : Socket(),
			m_endpoint(ep), m_socket(socket) {
		m_ip = boost::lexical_cast<std::string>(ep);
	}


	void Udp::start(errors::error_code& ec) {
		Socket::start(ec);
	}

	size_t Udp::write(const asio::mutable_buffer& buffer, errors::error_code &ec){
		boost::system::error_code e;
		size_t bytes_written = m_socket->send_to(buffer, m_endpoint, 0, e);
		if (e){
			//DEBUG_PRINT("Error occured! Error code = ", e.value(),". Message: " ,e.message());
			ec = errors::get_error("Udp::write", errors::SEND_ERROR, e.message());
		}
		return bytes_written;
	}

	void Udp::close() {
		Socket::close();
	}
}