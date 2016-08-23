#pragma once

#ifndef WS_SERVER_H
#define WS_SERVER_H

#include <string>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <thread>
#include <debug.h>
#include "client/client.h"
#include <cstdint>

namespace server{

	struct ServerData {
		std::string ip;
		int port;
		boost::asio::ip::tcp::acceptor* acceptor;

		ClientsManager::u_ptr clientManager;
	};

	void parse_from_args(int argc, const char* argv[], ServerData& serverData){
		using namespace boost::program_options;


		// Declare the supported options.
		options_description desc("Allowed options");
		desc.add_options()
				("help", "produce help message")
				("ip,ip", value<std::string>(), "ip sur laquelle écouter")
				("port,port", value<int>(), "port utilisé")
				;

		variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		if (vm.count("help")) {
			std::cout << desc << std::endl;
			return;
		}

		if (vm.count("ip")) {
			serverData.ip = vm["ip"].as<std::string>();
		} else {
			std::cout << "Ip was not set."<< std::endl;
		}

		if (vm.count("port")) {
			serverData.port = vm["port"].as<int>();
		} else {
			std::cout << "Port was not set."<< std::endl;
		}
	}


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
	void init(ServerData* data, Manager *m){
		using namespace boost::asio::ip;
		DEBUG_PRINT("MAX THREADS ", std::thread::hardware_concurrency() );
		errors::init_handler();

		try {
			boost::asio::io_service io_service;

			tcp::endpoint ep(boost::asio::ip::tcp::v4(), data->port);
			data->acceptor = new tcp::acceptor(io_service, ep);
			data->clientManager = ClientsManager::u_ptr(new ClientsManager(m));

			data->clientManager->init();

			if (data->clientManager->isAlive()){
				std::string ip = boost::lexical_cast<std::string>(ep);
				std::cout << "listening on : " << ip << std::endl;
				accept_loop(data);
			}

			io_service.run();
		}
		catch(std::exception& e){
			std::cerr << e.what() << std::endl;
		}

	}

	void close(ServerData* data){
		delete(data->acceptor);
	}


}

#endif // WS_SERVER_H
