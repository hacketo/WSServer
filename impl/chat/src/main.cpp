#include "server/server.h"
#include "Chat.h"

int main(int argc, const char *argv[]){

	config::DEFAULT_PARSER = protocol::packet::ParserType::NO;
	Chat* chat = new Chat();

	server::ServerData d;

	d.ip = "127.0.0.1";
	d.port = 9876;

	server::parse_from_args(argc, argv, d);

	server::init(&d, chat);


    return 0;
}