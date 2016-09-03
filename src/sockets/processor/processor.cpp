//
// Created by hacketo on 03/09/16.
//

#include "sockets/processor/processor.h"


namespace sockets {
namespace processor {


	template <class T>
	processor::processor<T>(T *sock) : m_sock(sock) {
	}

	template <class T>
	size_t processor<T>::send(std::string& msg, errors::error_code &ec){
		const char* cstr = msg.c_str();
		return send<const char*>(cstr, msg.size(), ec);
	}

	template <class T, typename BufferSequence>
	size_t processor<T>::send(BufferSequence& buffer, size_t size, errors::error_code &ec){
		return m_sock->write(asio::buffer(buffer, size), ec);
	}

	template <class T>
	void processor<T>::reset_buffer(){
		m_buffer.consume(m_buffer_size);
		m_buffer_size = 0;
	}

	template <class T>
	void processor<T>::consume(std::string& s){
		boost::asio::streambuf::const_buffers_type bufs = m_buffer.data();
		s = std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_end(bufs));
		reset_buffer();
	}

	template <class T>
	void processor<T>::on_receive(errors::error_code& ec) {
//		char *buffer;
//		size_t size;
//		consume(buffer,size);
		std::string msg;
		consume(msg);

		errors::error_code ec2;
		protocol::packet::Packet::u_ptr p = protocol::packet::Packet::u_ptr(new protocol::packet::Packet());
		protocol::packet::parse(p.get(), msg, ec2);

		if (!ec2) {
			m_sock->m_client->on_receive(p.get());
		} else {
			DEBUG_PRINT("error while parsing the response");
		}
		DEBUG_PRINT("recv [", msg.size(), "] : ", msg);
	}

	template <class T>
	void processor<T>::on_read(const boost::system::error_code &e, std::size_t bytes_read) {
		if (!e) {
			if (bytes_read > 0) {
				m_buffer_size += bytes_read;
				m_buffer.commit(bytes_read);
				if (bytes_read < max_buffer_len) {
					errors::error_code ec;
					on_receive(ec);
				} else {
					DEBUG_PRINT("recv increasing buffer !!");
				}
			} else {
				DEBUG_PRINT("recv NOTHING !! : ");
			}
			read_loop();
		} else {
			if (e.value() == boost::asio::error::eof) {
				m_sock->close();
			}
			DEBUG_PRINT("Error while recv : ", e.message());
		}
	}

}
}