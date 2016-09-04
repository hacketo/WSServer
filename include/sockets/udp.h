//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_UDP_H
#define WSSERVERLIB_UDP_H


#include "socket.h"
#include "sockets/processor/udp.h"

namespace sockets{

	class Udp : public Socket {

	public:
		Udp(asio::ip::udp::socket* socket,asio::ip::udp::endpoint ep);

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(error::code& ec);

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		void close();

		/**
		 * Write data to the socket, should only be called by processor::send
		 * @param buffer
		 * @param ec
		 * @return
		 */
		size_t write(const asio::const_buffers_1& buffer, error::code &ec);

	private:
		asio::ip::udp::endpoint m_endpoint;
		asio::ip::udp::socket* m_socket;

	};

}


#endif //WSSERVERLIB_UDP_H
