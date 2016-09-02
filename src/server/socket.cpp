//
// Created by hacketo on 01/09/16.
//

#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/lexical_cast.hpp>
#include "server/socket.h"
#include <client/client.h>

namespace sockets {

//<editor-fold desc="sockets">

	Socket::Socket() {

	}

	std::string Socket::get_ip() {
		return m_ip;
	}


	void Socket::start(errors::error_code& ec) {}

	void Socket::send(std::string &message, errors::error_code& ec) {}

	void Socket::close() {
		m_closed.store(true);
	}

	bool Socket::closed() {
		return m_closed.load();
	}

//</editor-fold>

//<editor-fold desc="tcp_socket">

	TcpSocket::TcpSocket(ip::tcp::socket *s) :
			Socket(), m_socket(std::unique_ptr<ip::tcp::socket>(s)) {
		m_processor = std::unique_ptr<tcp_processor>(new tcp_processor(this));
		m_ip = boost::lexical_cast<std::string>(m_socket->remote_endpoint());
	}

	ip::tcp::socket* TcpSocket::sock(){
		return m_socket.get();
	}

	void TcpSocket::start(errors::error_code& ec) {
		read_loop();
	}

	void TcpSocket::read_loop() {
		if (!closed()) {
			m_processor->process();
		}
	}

	void TcpSocket::send(std::string &message, errors::error_code& ec) {
		//todo: tcp send
	}

	void TcpSocket::close() {
		m_socket->close();
		Socket::close();
	}

//</editor-fold>

//<editor-fold desc="udp_socket">

	UdpSocket::UdpSocket(ip::udp::socket* socket, ip::udp::endpoint ep) : Socket(),
			m_endpoint(ep), m_socket(socket) {
		m_ip = boost::lexical_cast<std::string>(ep);
	}


	void UdpSocket::start(errors::error_code& ec) {


	}

	void UdpSocket::send(std::string &message, errors::error_code& e) {
		m_socket->async_send_to(boost::asio::buffer(message.c_str(), message.size()), m_endpoint,
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

	io_service& ServerSocket::io(){
		return m_ios;
	}

	bool ServerSocket::closed() {
		return m_closed.load();
	}

//</editor-fold>

//<editor-fold desc="server_udpsocket">

	Server_UdpSocket::Server_UdpSocket(ClientManager *clientManager, short port) :
			ServerSocket(clientManager), m_endpoint(ip::address_v4::any(), port),
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
			streambuf::mutable_buffers_type buf = buffer.get()->prepare(400);

			ip::udp::endpoint sender_endpoint;

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
			ServerSocket(clientManager), m_endpoint(ip::address_v4::any(), port),
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
			std::shared_ptr<ip::tcp::socket> sock(new ip::tcp::socket(m_ios));
			m_acceptor.async_accept(*sock.get(),
			   [this, sock](boost::system::error_code ec) {
				   on_accept(sock, ec);
			   }
			);
		}
		else{
			m_acceptor.close();
		}
	}

	void Server_TcpSocket::on_accept(std::shared_ptr<ip::tcp::socket> sock, const boost::system::error_code &ec) {

		loop();

		if (!ec) {
			Socket *s = get_new_socket(sock);
			m_clientManager->handle_new_socket(s);

			errors::error_code e;
			s->start(e);

			if (e){
				DEBUG_PRINT(e);
			}
		}
		else {
			std::cout<< "Error occured! Error code = "
					 << ec.value()
					 << ". Message: " <<ec.message();
		}
	}

	void Server_TcpSocket::close() {
		m_acceptor.close();
		ServerSocket::close();
	}

	Socket* Server_TcpSocket::get_new_socket(std::shared_ptr<ip::tcp::socket> sock){
		return new TcpSocket(sock.get());
	}

//</editor-fold>

	processor::processor(){
		reset_buffer();
	}

	void processor::reset_buffer(){
		buffer.reset(new boost::asio::streambuf);
		size_ = 0;
	}

	tcp_processor::tcp_processor(sockets::TcpSocket *tcp_sock) : processor(),
																 tcp_sock(tcp_sock), sock_(tcp_sock->sock()){

	}
	void tcp_processor::process(){

		streambuf::mutable_buffers_type buf = buffer.get()->prepare(400);

		sock_->async_read_some(buf,
		    [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
			    size_ = bytes_transferred;
			    if (!ec){

				    DEBUG_PRINT("recv : " );

				    read_loop();
			    }
			    else{

			    }
	    });

	}

	void tcp_processor::read_loop(){
		tcp_sock->read_loop();
	}



	httpprocessor::httpprocessor(sockets::TcpSocket *tcp_sock) : tcp_processor(tcp_sock), state(NONE){}


	void httpprocessor::process(){

		reset_buffer();

//		if (state == State::NONE) {
//			async_read_until(*sock_, *buffer.get(), "\r\n",
//			    [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
//				    size_ = bytes_transferred;
//				    if (!ec){
//					    state = READ_REQUEST;
//					    read_loop();
//				    }
//				    else{
//
//				    }
//		    });
//		}
//		if (state == State::READ_REQUEST){
//			async_read_until(*sock_, *buffer.get(), "\r\n\r\n",
//			    [this](const boost::system::error_code& ec,std::size_t bytes_transferred){
//				    size_ = bytes_transferred;
//				    if (!ec){
//					    state = READ_HEADER;
//					    read_loop();
//				    }
//				    else{
//				    }
//		    });
//		}

		if (state == State::READ_HEADER){
			state = NONE;
			read_loop();
		}
	}
}


