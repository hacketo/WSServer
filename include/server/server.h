#pragma once

#ifndef WS_SERVER_H
#define WS_SERVER_H

#include <string>

#include <boost/asio.hpp>
#include <thread>
#include <debug.h>
#include "sockets/socket.h"
#include "client/client.h"
#include <cstdint>

namespace server{

	enum Type {
		TCP, UDP, WS
	};

	struct ServerInfos {
		std::vector<std::pair<Type, short>> protocols;
	};

	class Server{

	public:
		Server(ServerInfos& infos, Manager *m);

		void start(errors::error_code& ec);

		void close();
	private:

		std::vector<std::unique_ptr<sockets::ServerSocket>> m_sockets;

		ClientManager::u_ptr m_clientManager;

	};
}

#endif // WS_SERVER_H
