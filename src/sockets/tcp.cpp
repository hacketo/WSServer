//
// Created by hacketo on 03/09/16.
//

#include <boost/lexical_cast.hpp>
#include "sockets/tcp.h"

namespace sockets {

	Tcp::Tcp(asio::io_service& io) :
			Socket(), m_socket(io) {
		m_processor = std::unique_ptr<processor::tcp>(new processor::tcp(this));
	}


	template <typename BufferSequence>
	size_t Tcp::send(BufferSequence& buffer, size_t size, errors::error_code &ec){
		return m_processor->send<BufferSequence>(buffer, size, ec);
	}


	size_t Tcp::send(std::string& message, errors::error_code& ec) {
		return m_processor->send(message, ec);
	}

	size_t Tcp::write(const asio::mutable_buffer& buffer, errors::error_code &ec){
		boost::system::error_code e;
		size_t bytes_written = m_socket.write_some(buffer, e);
		if (e){
			//DEBUG_PRINT("Error occured! Error code = ", e.value(),". Message: " ,e.message());
			ec = errors::get_error("TcpSocket::write", errors::SEND_ERROR, e.message());
		}
		return bytes_written;
	}

	void Tcp::start(errors::error_code& ec) {
		Socket::start(ec);
		m_ip = boost::lexical_cast<std::string>(m_socket.remote_endpoint());
		read_loop();
	}

	void Tcp::read_loop() {
		if (!closed()) {
			m_processor->process();
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