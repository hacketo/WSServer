//
// Created by hacketo on 01/09/16.
//

#include "server.h"
#include "sockets/server/tcp.h"
#include "sockets/server/udp.h"


Server::Server(ServerInfos &infos, Manager *m) {

	m_clientManager = ClientManager::u_ptr(new ClientManager(m));

	for(auto& pro : infos.protocols){

		sockets::server::ServerSocket* s;

		switch(pro.first){

			case Type::TCP:
				s = new sockets::server::Tcp(m_clientManager.get(), pro.second);
				break;

			case Type::UDP:
				//s = new sockets::server_udpsocket(m_clientManager.get(), m_ios);
				break;

			case Type::WS:
				//s = new sockets::server_tcpsocket(m_clientManager, m_ios);
				break;

		}

		if (s){
			m_sockets.push_back(std::unique_ptr<sockets::server::ServerSocket>(s));
		}
	}
}

void Server::start(error::code& ec) {

	DEBUG_PRINT("MAX THREADS ", std::thread::hardware_concurrency() );
	error::init_handler();

	m_clientManager->init();

	if (m_clientManager->isAlive()){

		unsigned int thread_per_serversocket = 1;//thread_pool_size / m_sockets.size();

		boost::thread_group threads;

		try {
			for (auto &sockets : m_sockets) {
				sockets->start(ec);

				// Create specified number of threads and
				// add them to the pool.
				for (unsigned int i = 0; i < thread_per_serversocket; i++) {
					threads.create_thread(boost::bind(&boost::asio::io_service::run, &sockets->io()));
				}

			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			threads.join_all();
		}
		catch(std::exception& e){
			std::cerr << e.what() << std::endl;
		}
	}
}

void Server::close() {
	m_clientManager->close();
}

