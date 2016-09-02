//
// Created by hacketo on 01/09/16.
//

#ifndef WSSERVERLIB_SOCKET_H
#define WSSERVERLIB_SOCKET_H


#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio.hpp>
#include "errors.h"
#include "protocol/constant.h"


class ClientManager;
class Client;

using namespace boost::asio;

namespace sockets {

	class processor;
	class tcp_processor;

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

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(errors::error_code& ec);

	protected:
		std::string m_ip;
		std::atomic<bool> m_closed;
	};

	class TcpSocket : public Socket {
	public:
		TcpSocket(ip::tcp::socket *s);

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

		ip::tcp::socket* sock();


		void read_loop();
	private:

		std::unique_ptr<ip::tcp::socket> m_socket;

		std::unique_ptr<tcp_processor> m_processor;

	};

	class UdpSocket : public Socket {
		friend class udp_processor;
	public:
		UdpSocket(ip::udp::socket* socket,ip::udp::endpoint ep);

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
		ip::udp::endpoint m_endpoint;
		ip::udp::socket* m_socket;
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

		io_service& io();

	protected:

		virtual void loop();

		ClientManager *m_clientManager;

		io_service m_ios;

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

		ip::udp::endpoint m_endpoint;
		ip::udp::socket m_socket;
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
		virtual Socket* get_new_socket(std::shared_ptr<ip::tcp::socket> sock);

	private:
		ip::tcp::endpoint m_endpoint;
		boost::asio::ip::tcp::acceptor m_acceptor;

		void on_accept(std::shared_ptr<ip::tcp::socket> sock, const boost::system::error_code &ec);
	};



	class processor {
	public:
		processor();
	protected :
		std::unique_ptr<streambuf> buffer;
		uint64_t size_;
		void reset_buffer();
	};

	class tcp_processor : public processor {
	public:
		tcp_processor(TcpSocket *tcp_sock);
		virtual void process();
	protected:
		void read_loop();

		TcpSocket* tcp_sock;
		ip::tcp::socket* sock_;
	};

	class httpprocessor : public tcp_processor{

		enum State {
			NONE, READ_REQUEST, READ_HEADER
		};

		httpprocessor(TcpSocket *tcp_sock);

		virtual void process();

	private :
		State state;
	};
}

#endif //WSSERVERLIB_SOCKET_H
