//
// Created by hacketo on 03/09/16.
//

#include "sockets/processor/processor.h"
#include "sockets/socket.h"
#include "protocol/packet.h"
#include "client/client.h"
#include "debug.h"

namespace sockets {
namespace processor {



	processor::processor(Socket *sock) : m_sock(sock) {
	}


	size_t processor::send(std::string& msg, error::code &ec){
		const char* cstr = msg.c_str();
		return send<const char*>(cstr, msg.size(), ec);
	}

	template <typename BufferSequence>
	size_t processor::send(BufferSequence& buffer, size_t size, error::code &ec){
		return m_sock->write(asio::buffer(buffer, size), ec);
	}

	void processor::reset_buffer(){
		m_buffer.consume(m_buffer_size);
		m_buffer_size = 0;
	}

	void processor::consume(std::string& s){
		boost::asio::streambuf::const_buffers_type bufs = m_buffer.data();
		s = std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_end(bufs));
		reset_buffer();
	}


	void processor::on_receive(error::code& ec) {
//		char *buffer;
//		size_t size;
//		consume(buffer,size);
		std::string msg;
		consume(msg);

		error::code ec2;
		protocol::packet::Packet::u_ptr p = protocol::packet::Packet::u_ptr(new protocol::packet::Packet());
		protocol::packet::parse(p.get(), msg, ec2);

		if (!ec2) {
			m_sock->client()->on_receive(p.get());
		} else {
			DEBUG_PRINT("error while parsing the response");
		}
		DEBUG_PRINT("recv [", msg.size(), "] : ", msg);
	}


	void processor::on_read(const boost::system::error_code &e, std::size_t bytes_read) {
		if (!e) {
			if (bytes_read > 0) {
				m_buffer_size += bytes_read;
				m_buffer.commit(bytes_read);
				if (bytes_read < max_buffer_len) {
					error::code ec;
					on_receive(ec);
				} else {
					DEBUG_PRINT("recv increasing buffer !!");
				}
			} else {
				DEBUG_PRINT("recv NOTHING !! : ");
			}
		}
	}


}
}