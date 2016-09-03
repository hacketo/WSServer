//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_SERVER_UDP_H
#define WSSERVERLIB_SERVER_UDP_H


#include "socket.h"
#include "sockets/udp.h"

namespace sockets {
namespace server {

	using namespace boost;

	class Udp : public ServerSocket {

	public :

		Udp(ClientManager *clientManager, short port);
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
		//virtual sockets* get_new_socket(std::shared_ptr<ip::udp::sockets> sock);
	private:

		asio::ip::udp::endpoint m_endpoint;
		asio::ip::udp::socket m_socket;
	};

}


#endif //WSSERVERLIB_SERVER_UDP_H
