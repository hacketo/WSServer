#pragma once

#ifndef WS_SERVER_H
#define WS_SERVER_H

#include <string>

#include <boost/asio.hpp>
#include "../client/client.h"

namespace server{

	struct ServerData {
		std::string ip;
		unsigned short port;
		boost::asio::ip::tcp::acceptor* acceptor;

		ClientsManager::u_ptr clientManager;
	};


	//ServerData* serverData;


	void accept_loop(ServerData* data);

	/**
	 * Démarre la gestion du nouveau client
	 */
	void handle_accept(ServerData* data, Client::s_ptr new_connection, const boost::system::error_code& error){
		if (!error){
			// todo: new thread
			new_connection->start();
		}

		accept_loop(data);
	}

	/**
	 * Récupère un nouveau client asap
	 * et l'assigne des qu'une connexion entre
	 */
	void accept_loop(ServerData* data){
		Client::s_ptr new_connection = data->clientManager->new_client(
				data->acceptor->get_io_service());
		data->acceptor->async_accept(new_connection->socket(),
								   boost::bind(&handle_accept,
											   data, new_connection,
											   boost::asio::placeholders::error));
	}

	/**
	 * Initialise le clients manager et démarre l'acceptation des clients
	 * @param data
	 * @param io_service
	 * @param manager
	 */
	void init(ServerData* data, boost::asio::io_service &io_service,
			  Manager *m = new Manager(new ModulesManager)){

		data->acceptor = new boost::asio::ip::tcp::acceptor(io_service,
										boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), data->port));
		data->clientManager = ClientsManager::u_ptr(new ClientsManager(m));
		accept_loop(data);
	}
}

#endif // WS_SERVER_H
