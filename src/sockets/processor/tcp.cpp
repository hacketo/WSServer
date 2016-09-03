//
// Created by hacketo on 03/09/16.
//

#include <boost/bind.hpp>

#include "sockets/processor/tcp.h"
#include "sockets/tcp.h"

namespace sockets {
namespace processor {

	tcp::tcp(Tcp * sock) : processor<Tcp>(sock){

	}

	void tcp::process() {
		asio::streambuf::mutable_buffers_type mutableBuffer = m_buffer.prepare(max_buffer_len);
		asio::async_read(m_sock->sock(), mutableBuffer, asio::transfer_at_least(1),
						 boost::bind(&processor::on_read, this, _1, _2));
	}

	void tcp::read_loop() {
		m_sock->read_loop();
	}

}
}