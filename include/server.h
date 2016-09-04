#pragma once

#ifndef WS_SERVER_H
#define WS_SERVER_H

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <cstdint>

#include <debug.h>
#include "sockets/server/socket.h"
#include "client/clientmanager.h"

class Server{

public:
	enum Type {
		TCP, UDP, WS
	};

	struct ServerInfos {
		std::vector<std::pair<Type, short>> protocols;
	};

	Server(ServerInfos& infos, Manager *m);

	void start(error::code& ec);

	void close();
private:

	std::vector<std::unique_ptr<sockets::server::ServerSocket>> m_sockets;

	ClientManager::u_ptr m_clientManager;

};

#endif // WS_SERVER_H
