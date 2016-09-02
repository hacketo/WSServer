//
// Created by hacketo on 01/09/16.
//

#ifndef WSSERVERLIB_SOCKET_H
#define WSSERVERLIB_SOCKET_H


#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio.hpp>
#include "server/errors.h"
#include "protocol/constant.h"


class ClientManager;
class Client;

using namespace boost;

namespace sockets {

	class processor;

	class Socket {
		friend class processor;
	public:

		typedef std::unique_ptr<Socket> u_ptr;

		Socket();

		std::string get_ip();

		virtual void send(std::string &message, errors::error_code& ec);

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		virtual void close();

		/**
		 * @return true if the socket is closed
		 */
		bool closed();

		void bind_client(Client *c, errors::error_code& ec);

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(errors::error_code& ec);

	protected:

		Client* m_client;

		std::string m_ip;
		std::atomic<bool> m_closed;
		std::atomic<bool> m_clientBinded;
	};

	class TcpSocket : public Socket {
	public:
		TcpSocket(asio::io_service& io);

		/**
		 * Send a packet througt the socket
		 */
		virtual void send(char* buffer, size_t size, errors::error_code& ec);

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		virtual void close();

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(errors::error_code& ec);

		asio::ip::tcp::socket& sock();


		void read_loop();
	private:

		asio::ip::tcp::socket m_socket;

		std::unique_ptr<processor> m_processor;

	};

	class UdpSocket : public Socket {
		friend class udp_processor;
	public:
		UdpSocket(asio::ip::udp::socket* socket,asio::ip::udp::endpoint ep);

		/**
		 * Send a packet througt the socket
		 */

		virtual void send(std::string &message, errors::error_code& ec);

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		virtual void close();

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(errors::error_code& ec);

	private:
		asio::ip::udp::endpoint m_endpoint;
		asio::ip::udp::socket* m_socket;
	};



	class ServerSocket {

	public :

		/**
		 * Start the possibility of this socket to receive messages
		 * @return
		 */
		virtual void start(errors::error_code& ec);

		ServerSocket(ClientManager *clientManager);

		/**
		 * Close the socket
		 * Will not accept any client after
		 */
		virtual void close();

		bool closed();

		asio::io_service& io();

	protected:

		virtual void loop();

		ClientManager *m_clientManager;

		asio::io_service m_ios;

		std::atomic<bool> m_closed;
		std::atomic<bool> m_started;

	};

	class Server_UdpSocket : public ServerSocket {

	public :

		Server_UdpSocket(ClientManager *clientManager, short port);
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

	class Server_TcpSocket : public ServerSocket {

	public :

		Server_TcpSocket(ClientManager *clientManager, short port);

		~Server_TcpSocket();

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
		virtual TcpSocket* get_new_socket();

	private:
		asio::ip::tcp::endpoint m_endpoint;
		asio::ip::tcp::acceptor m_acceptor;

		void on_accept(TcpSocket* sock, const boost::system::error_code &ec);
	};

}

#endif //WSSERVERLIB_SOCKET_H
