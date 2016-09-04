//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_H
#define WSSERVERLIB_PROCESSOR_H


#include <cstdio>
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>

#include "error/error.h"
#include "protocol/constant.h"

namespace sockets {
	class Socket;

namespace processor {

	using namespace boost;

	/**
	 * A processor is used to wrap a socket around a protocol
	 * It mean that it can transform a message before sending it to the socket,
	 * and transform a message comming from the client before handling the message
	 */
	class processor {
	public:
		processor(Socket* socket);

		/**
		 * Send a message to the socket
		 * Called by Socket::send
		 * @param buffer
		 * @param size
		 * @param ec
		 * @return
		 */
		template<typename BufferSequence>
		size_t send(BufferSequence &buffer, size_t size, error::code &ec);

		/**
		 * Send a string to the socket
		 * Called by Socket::send
		 * @param msg
		 * @param ec
		 * @return
		 */
		virtual size_t send(std::string &msg, error::code &ec);

	protected:

		/**
		 * Called when we have data from client
		 * in this callback you can read data from the buffer and consume it to reset the buffer
		 */
		virtual void on_receive(error::code &ec);

		template<typename BufferSequence>
		void consume(BufferSequence *buffer) {
			asio::streambuf::const_buffers_type bufs = m_buffer.data();
			std::copy(asio::buffers_begin(bufs), asio::buffers_end(bufs), buffer);
			reset_buffer();
		}

		void consume(std::string &s);

		asio::streambuf m_buffer;

		size_t m_buffer_size;
		size_t max_buffer_len = protocol::constant::max_buffer_size;

		/**
		 * Called right after we received some data from the socket
		 * @param ec
		 * @param bytes_read
		 */
		virtual void on_read(const boost::system::error_code &ec, std::size_t bytes_read);

	private:
		Socket *m_sock;

		void reset_buffer();


	};
}
}


#endif //WSSERVERLIB_PROCESSOR_H
