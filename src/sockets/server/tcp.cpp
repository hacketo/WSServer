//
// Created by hacketo on 03/09/16.
//

#include <boost/lexical_cast.hpp>
#include "sockets/server/tcp.h"
#include "client/clientmanager.h"

namespace sockets {
namespace server {

	Tcp::Tcp(ClientManager *clientManager, short port) :
			ServerSocket(clientManager), m_endpoint(asio::ip::address_v4::any(), port),
			m_acceptor(m_ios, m_endpoint.protocol()) {

	}

	void Tcp::start(error::code &ec) {

		boost::system::error_code e;
		m_acceptor.bind(m_endpoint, e);
		m_acceptor.listen(50); // todo: max client

		if (e) {
			error::get_code(ec, error::SOCKET_CANT_BIND, "Failed to bind the acceptor sockets.", e);
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
	void Tcp::loop() {
		if (!m_closed.load()) {
			sockets::Tcp *tcpSocket = get_new_socket();
			m_acceptor.async_accept(tcpSocket->sock(),
				[this, tcpSocket](boost::system::error_code ec) {
					on_accept(tcpSocket, ec);
				}
			);
		}
		else {
			m_acceptor.close();
		}
	}

	void Tcp::on_accept(sockets::Tcp *sock, const boost::system::error_code &ec) {

		loop();

		if (!ec) {
			m_clientManager->handle_new_socket(sock);
			error::code e;
			sock->start(e);

			if (e) {
				DEBUG_PRINT(e);
			}
		}
		else {
			sock->close();
			delete sock;
			DEBUG_PRINT("Error occured! Error value = ", ec.value(), ". Message: ", ec.message());
		}
	}

	void Tcp::close() {
		m_acceptor.close();
		ServerSocket::close();
	}

	sockets::Tcp *Tcp::get_new_socket() {
		return new sockets::Tcp(m_ios);
	}

}
}