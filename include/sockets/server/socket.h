//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_SERVER_SOCKET_H
#define WSSERVERLIB_SERVER_SOCKET_H


#include <error/error.h>
#include <boost/asio.hpp>
#include <atomic>

class ClientManager;


namespace sockets {
namespace server {

	using namespace boost;

	class ServerSocket {

	public :

		/**
		 * Start the possibility of this socket to receive messages
		 * @return
		 */
		virtual void start(error::code &ec);

		ServerSocket(ClientManager *clientManager);

		/**
		 * Close the socket
		 * Will not accept any client after
		 */
		virtual void close();

		bool closed();

		asio::io_service &io();

	protected:

		virtual void loop();

		ClientManager *m_clientManager;

		asio::io_service m_ios;

		std::atomic<bool> m_closed;
		std::atomic<bool> m_started;

	};

}
}


#endif //WSSERVERLIB_SERVER_SOCKET_H
