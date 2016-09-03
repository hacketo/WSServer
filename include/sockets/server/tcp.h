//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_SERVER_TCP_H
#define WSSERVERLIB_SERVER_TCP_H


#include "sockets/tcp.h"
#include "socket.h"

namespace sockets {
namespace server {

	using namespace boost;

	class Tcp : public ServerSocket {

	public :

		Tcp(ClientManager *clientManager, short port);

		/**
		 * Start the client listening
		 * @return
		 */
		virtual void start(errors::error_code& ec);

		/**
		 * Close the socket
		 * Will not accept any client after
		 */
		virtual void close();

	protected:
		virtual void loop();
		virtual sockets::Tcp* get_new_socket();

	private:
		asio::ip::tcp::endpoint m_endpoint;
		asio::ip::tcp::acceptor m_acceptor;

		void on_accept(sockets::Tcp* sock, const boost::system::error_code &ec);
	};

}
}

#endif //WSSERVERLIB_SERVER_TCP_H
