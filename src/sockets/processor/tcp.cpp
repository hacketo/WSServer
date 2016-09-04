//
// Created by hacketo on 03/09/16.
//

#include <boost/bind.hpp>

#include "sockets/processor/tcp.h"
#include "sockets/tcp.h"
#include "debug.h"

namespace sockets {
namespace processor {

	tcp::tcp(Tcp * sock) : processor(sock){
		m_sock = sock;
	}

	void tcp::async_read() {
		asio::streambuf::mutable_buffers_type mutableBuffer = m_buffer.prepare(max_buffer_len);
		asio::async_read(m_sock->sock(), mutableBuffer, asio::transfer_at_least(1),
						 boost::bind(&tcp::on_read, this, _1, _2));
	}

	void tcp::read_loop() {
		m_sock->read_loop();
	}

	void tcp::on_read(const boost::system::error_code &e, std::size_t bytes_read){
		processor::on_read(e, bytes_read);
		if(e){
			if (e.value() == boost::asio::error::eof) {
				m_sock->close();
			}
			DEBUG_PRINT("Error while recv : ", e.message());
		}
		else{
			read_loop();
		}
	}

}
}