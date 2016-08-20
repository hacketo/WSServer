//
// Created by hacketo on 12/08/16.
//

#include <iostream>
#include <boost/asio.hpp>
#include "server/server.h"
#include "client/manager/statebased_manager.h"
#include "impl/RED.h"
#include "modules/base_module.h"
#include "modules/pub_sub.h"

int main(){


	StateBasedManager *manager = new StateBasedManager(new ModulesManager());
	manager->initStates({new VoidState(manager), new MainState(manager)});

	server::ServerData d;
	d.ip = "127.0.0.1";
	d.port = 9876;

	try {
		boost::asio::io_service io_service;
		server::init(&d, io_service, manager);
		io_service.run();
	}
	catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}

	server::close(&d);

	return 0;
}