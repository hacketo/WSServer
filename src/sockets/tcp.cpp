//
// Created by hacketo on 03/09/16.
//

#include <boost/lexical_cast.hpp>
#include "sockets/tcp.h"
#include "client/client.h"

namespace sockets {

	Tcp::Tcp(asio::io_service& io) :
			Socket(), m_socket(io) {
	}


	template <typename BufferSequence>
	size_t Tcp::send(BufferSequence& buffer, size_t size, error::code &ec){
		assert(m_procBinded.load());
		return m_processor->send<BufferSequence>(buffer, size, ec);
	}


	size_t Tcp::send(std::string& message, error::code& ec) {
		assert(m_procBinded.load());
		return m_processor->send(message, ec);
	}

	size_t Tcp::write(const asio::const_buffers_1& buffer, error::code &ec){
		boost::system::error_code e;
		size_t bytes_written = m_socket.write_some(buffer, e);
		if (e){
			//DEBUG_PRINT("Error occured! Error value = ", e.value(),". Message: " ,e.message());
			error::get_code(ec, "TcpSocket::write", error::SEND_ERROR, e.message());
		}
		return bytes_written;
	}

	void Tcp::start(error::code& ec) {
		Socket::start(ec);
		m_ip = boost::lexical_cast<std::string>(m_socket.remote_endpoint());
		read_loop();
	}

	void Tcp::read_loop() {
		if (!closed()) {
			assert(m_procBinded.load());
			m_processor->async_read();
		}
	}

	void Tcp::close() {
		m_socket.close();
		Socket::close();
		m_client->on_close();
	}

	asio::ip::tcp::socket& Tcp::sock(){
		return m_socket;
	}

}