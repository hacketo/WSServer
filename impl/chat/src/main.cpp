#include <iostream>
#include <boost/asio.hpp>
#include "server/server.h"
#include "Chat.h"


int main(){

#ifdef USE_MODULES
	std::cout << "modules : " << USE_MODULES << std::endl;
#endif
#ifdef USE_SESSIONS
	std::cout << "sessions : "<<USE_SESSIONS<<std::endl;
#endif


	config::DEFAULT_PARSER = protocol::packet::ParserType::NO;
	Chat* chat = new Chat();

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