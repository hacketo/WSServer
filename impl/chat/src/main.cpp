#include "server.h"

#include "Chat.h"

int main(int argc, const char *argv[]){

	config::DEFAULT_PARSER = protocol::packet::ParserType::NO;
	Chat* chat = new Chat();

	Server::ServerInfos d;
	d.protocols = {std::make_pair(Server::TCP, 9876)};

	Server s(d, chat);

	error::code ec;

	try {
		s.start(ec);
	}
	catch(std::exception& e){
		std::cout << "Error occured " << e.what() << std::endl;
	}
	s.close();


    return 0;
}