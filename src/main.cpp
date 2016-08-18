#include <iostream>
#include <boost/asio.hpp>
#include "server/server.h"
#include "impl/Chat.h"
#include "server/config.h"


int main(){

	config::DEFAULT_PARSER = protocol::packet::ParserType::NO;
	Chat* chat = new Chat(new ModulesManager());

	try {
		boost::asio::io_service io_service;

		server::ServerData d;
		d.ip = "127.0.0.1";
		d.port = 9876;

		server::init(&d, io_service, chat);
		
		io_service.run();
    }
	catch(std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	
    return 0;
}