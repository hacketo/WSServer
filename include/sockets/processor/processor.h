//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_H
#define WSSERVERLIB_PROCESSOR_H


#include <cstdio>
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>

#include "server/errors.h"
#include "protocol/constant.h"

namespace sockets {
namespace processor {

	using namespace boost;

	/**
	 * use to process a message from a Socket
	 */
	template<class T>
	class processor {
	public:
		processor(T *tcp_sock);

		/**
		 * Called to wait for a mesage from this socket
		 * makes no sence so use this with udp sockets
		 */
		virtual void process() = 0;

		/**
		 * Called to  try to restart the async read from socket,
		 */
		virtual void read_loop() = 0;

		/**
		 * Send a message to the socket
		 * Called by Socket::send
		 * @param buffer
		 * @param size
		 * @param ec
		 * @return
		 */
		template<typename BufferSequence>
		size_t send(BufferSequence &buffer, size_t size, errors::error_code &ec);

		/**
		 * Send a string to the socket
		 * Called by Socket::send
		 * @param msg
		 * @param ec
		 * @return
		 */
		virtual size_t send(std::string &msg, errors::error_code &ec);

	protected:

		/**
		 * Called when we have data from client
		 * in this callback you can read data from the buffer and consume it to reset the buffer
		 */
		virtual void on_receive(errors::error_code &ec);

		template<typename BufferSequence>
		void consume(BufferSequence *buffer) {
			asio::streambuf::const_buffers_type bufs = m_buffer.data();
			std::copy(asio::buffers_begin(bufs), asio::buffers_end(bufs), buffer);
			reset_buffer();
		}

		void consume(std::string &s);

		T *m_sock;

		asio::streambuf m_buffer;

		size_t m_buffer_size;
		size_t max_buffer_len = protocol::constant::max_buffer_size;

		void on_read(const boost::system::error_code &ec, std::size_t bytes_read);

	private:
		void reset_buffer();


	};
}
}


#endif //WSSERVERLIB_PROCESSOR_H
