//
// Created by hacketo on 01/09/16.
//

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include "client/client.h"
#include "sockets/socket.h"

namespace sockets {

	// see :http://stackoverflow.com/questions/22285733/excessive-message-size-with-boost-asio-and-ssl


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


	Client* Socket::client(){
		return m_client;
	}


	void Socket::start(errors::error_code& ec) {
		m_client->on_enter();
	}


	void Socket::close() {
		m_closed.store(true);
	}


	bool Socket::closed() const {
		return m_closed.load();
	}

}


