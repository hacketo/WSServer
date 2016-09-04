//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_TCP_H
#define WSSERVERLIB_TCP_H

#include "socket.h"
#include "sockets/processor/tcp.h"


namespace sockets{

	class Tcp : public Socket {
	public:
		Tcp(asio::io_service& io);

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		void start(error::code& ec);

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		void close();


		size_t send(std::string &message, error::code& ec);
		template <typename BufferSequence>
		size_t send(BufferSequence& buffer, size_t size, error::code &ec);


		/**
		 * Write data to the socket, should only be called by processor::send
		 * @param buffer
		 * @param ec
		 * @return
		 */
		size_t write(const asio::const_buffers_1& buffer, error::code &ec);

		void read_loop();

		asio::ip::tcp::socket& sock();

	private:
		asio::ip::tcp::socket m_socket;

	};

}


#endif //WSSERVERLIB_TCP_H
