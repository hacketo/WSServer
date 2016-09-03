//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_TCP_H
#define WSSERVERLIB_TCP_H

#include "socket.h"
#include "sockets/processor/tcp.h"


namespace sockets{

	class Tcp : public Socket<processor::tcp> {
	public:
		Tcp(asio::io_service& io);

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		void start(errors::error_code& ec);

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		void close();


		size_t send(std::string &message, errors::error_code& ec);
		template <typename BufferSequence>
		size_t send(BufferSequence& buffer, size_t size, errors::error_code &ec);


		/**
		 * Write data to the socket, should only be called by processor::send
		 * @param buffer
		 * @param ec
		 * @return
		 */
		size_t write(const asio::mutable_buffer& buffer, errors::error_code &ec);

		void read_loop();

		asio::ip::tcp::socket& sock();

	private:
		asio::ip::tcp::socket m_socket;
		processor::processor<Tcp> m_processor;

	};

}


#endif //WSSERVERLIB_TCP_H
