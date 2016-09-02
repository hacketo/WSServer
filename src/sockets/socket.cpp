//
// Created by hacketo on 01/09/16.
//

#include <boost/asio.hpp>
#include "sockets/processor.h"
#include <boost/lexical_cast.hpp>
#include "sockets/socket.h"
#include <client/client.h>

namespace sockets {

	// see :http://stackoverflow.com/questions/22285733/excessive-message-size-with-boost-asio-and-ssl

//<editor-fold desc="sockets">

	Socket::Socket() {
		m_closed.store(false);
		m_clientBinded.store(false);
	}

	std::string Socket::get_ip() {
		return m_ip;
	}


	void Socket::bind_client(Client* c, errors::error_code& ec) {
		if (!m_clientBinded.load()){
			m_client = c;
			m_clientBinded.store(true);
		}
		else{
			ec = errors::get_error("Socket",errors::SOCKET_ALREADY_BINDED, "Can't bind this socket to a new client");
		}

	}

	void Socket::start(errors::error_code& ec) {
		m_client->on_enter();
	}

	void Socket::send(std::string &message, errors::error_code& ec) {}

	void Socket::close() {
		m_closed.store(true);
	}

	bool Socket::closed() {
		return m_closed.load();
	}

//</editor-fold>

//<editor-fold desc="tcp_socket">

	TcpSocket::TcpSocket(asio::io_service& io) :
			Socket(), m_socket(io) {
		m_processor = std::unique_ptr<processor>(new processor(this));
	}

	asio::ip::tcp::socket& TcpSocket::sock(){
		return m_socket;
	}

	void TcpSocket::start(errors::error_code& ec) {
		Socket::start(ec);
		m_ip = boost::lexical_cast<std::string>(m_socket.remote_endpoint());
		read_loop();
	}

	void TcpSocket::read_loop() {
		if (!closed()) {
			m_processor->process();
		}
	}

	void TcpSocket::send(char* buffer, size_t size, errors::error_code& ec) {
		//todo: tcp send
		m_processor->send(buffer, size);

	}

	void TcpSocket::close() {
		m_socket.close();
		Socket::close();
		m_client->on_close();
	}

//</editor-fold>

//<editor-fold desc="udp_socket">

	UdpSocket::UdpSocket(asio::ip::udp::socket* socket, asio::ip::udp::endpoint ep) : Socket(),
			m_endpoint(ep), m_socket(socket) {
		m_ip = boost::lexical_cast<std::string>(ep);
	}


	void UdpSocket::start(errors::error_code& ec) {
		Socket::start(ec);

	}

	void UdpSocket::send(std::string &message, errors::error_code& e) {
		m_socket->async_send_to(asio::buffer(message.c_str(), message.size()), m_endpoint,
			[this](boost::system::error_code ec, std::size_t bytes_sent){
				if (ec){
					close();
				}
		});
	}

	void UdpSocket::close() {
		m_socket->close();
		Socket::close();
	}

//</editor-fold>

//<editor-fold desc="serversocket">

	ServerSocket::ServerSocket(ClientManager *clientManager) :
			m_clientManager(clientManager) {
	}

	void ServerSocket::start(errors::error_code& ec) {
		m_started.store(true);
		loop();
	}

	void ServerSocket::loop() {}

	void ServerSocket::close() {
		m_closed.store(true);
	}

	asio::io_service& ServerSocket::io(){
		return m_ios;
	}

	bool ServerSocket::closed() {
		return m_closed.load();
	}

//</editor-fold>

//<editor-fold desc="server_udpsocket">

	Server_UdpSocket::Server_UdpSocket(ClientManager *clientManager, short port) :
			ServerSocket(clientManager), m_endpoint(asio::ip::address_v4::any(), port),
			m_socket(m_ios, m_endpoint.protocol()) {

	}

	void Server_UdpSocket::start(errors::error_code& ec) {
		boost::system::error_code e;
		m_socket.bind(m_endpoint, e);

		if (e) {
			ec = errors::error_code("Failed to bind the sockets.", e);
		}
		else {
			std::string ip = boost::lexical_cast<std::string>(m_endpoint);
			std::cout << "listening on : " << ip << std::endl;
			ServerSocket::start(ec);
		}

	}

	void Server_UdpSocket::close() {
		m_socket.close();
		ServerSocket::close();
	}

	void Server_UdpSocket::loop() {
		if (!m_closed.load()) {
			std::unique_ptr<boost::asio::streambuf> buffer(new boost::asio::streambuf);
			asio::streambuf::mutable_buffers_type buf = buffer.get()->prepare(400);

			asio::ip::udp::endpoint sender_endpoint;

			m_socket.async_receive_from(buf, sender_endpoint,
				[this, sender_endpoint](boost::system::error_code ec, std::size_t bytes_recvd) {
					if (!ec && bytes_recvd > 0) {

						UdpSocket *s = new UdpSocket(&m_socket, sender_endpoint);

						m_clientManager->handle_new_socket(s);
					} else {
						loop();
					}
			});
		}
		else{

		}
	}

//</editor-fold>

//<editor-fold desc="server_tcpsocket">

	Server_TcpSocket::Server_TcpSocket(ClientManager *clientManager, short port) :
			ServerSocket(clientManager), m_endpoint(asio::ip::address_v4::any(), port),
			m_acceptor(m_ios, m_endpoint.protocol()) {

	}

	void Server_TcpSocket::start(errors::error_code& ec) {

		boost::system::error_code e;
		m_acceptor.bind(m_endpoint, e);
		m_acceptor.listen(50); // todo: max client

		if (e) {
			ec = errors::error_code("Failed to bind the acceptor sockets.", e);
		}
		else {
			std::string ip = boost::lexical_cast<std::string>(m_endpoint);
			std::cout << "listening on : " << ip << std::endl;
			ServerSocket::start(ec);
		}
	}


	/**
	 * Accepte un nouveau client
	 */
	void Server_TcpSocket::loop() {
		if (!m_closed.load()) {
			TcpSocket* tcpSocket = get_new_socket();
			m_acceptor.async_accept(tcpSocket->sock(),
			   [this, tcpSocket](boost::system::error_code ec) {
				   on_accept(tcpSocket, ec);
			   }
			);
		}
		else{
			m_acceptor.close();
		}
	}

	void Server_TcpSocket::on_accept(TcpSocket* sock, const boost::system::error_code &ec) {

		loop();

		if (!ec) {
			m_clientManager->handle_new_socket(sock);
			errors::error_code e;
			sock->start(e);

			if (e){
				DEBUG_PRINT(e);
			}
		}
		else {
			sock->close();
			delete sock;
			std::cout<< "Error occured! Error code = "
					 << ec.value()
					 << ". Message: " <<ec.message();
		}
	}

	void Server_TcpSocket::close() {
		m_acceptor.close();
		ServerSocket::close();
	}

	TcpSocket* Server_TcpSocket::get_new_socket(){
		return new TcpSocket(m_ios);
	}

//</editor-fold>

}


