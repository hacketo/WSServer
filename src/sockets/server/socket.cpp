//
// Created by hacketo on 03/09/16.
//

#include "sockets/server/socket.h"
#include "client/client.h"

namespace sockets {
namespace server {

	ServerSocket::ServerSocket(ClientManager *clientManager) :
			m_clientManager(clientManager) {
	}

	void ServerSocket::start(error::code &ec) {
		m_started.store(true);
		loop();
	}

	void ServerSocket::loop() {}

	void ServerSocket::close() {
		m_closed.store(true);
	}

	asio::io_service &ServerSocket::io() {
		return m_ios;
	}

	bool ServerSocket::closed() {
		return m_closed.load();
	}

}
}