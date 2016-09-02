#include "server/server.h"
#include "Chat.h"

int main(int argc, const char *argv[]){

	config::DEFAULT_PARSER = protocol::packet::ParserType::NO;
	Chat* chat = new Chat();

	server::ServerInfos d;
	d.protocols = {std::make_pair(server::TCP, 9876)};


	server::Server s(d, chat);

	errors::error_code ec;

	try {
		s.start(ec);
	}
	catch(auto e){

	}
	s.close();


    return 0;
}