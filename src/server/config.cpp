//
// Created by hacketo on 12/08/16.
//

#include "config.h"


namespace config {

	std::string SERVER_NAME = "ServerName";

	std::string HTTP_ORIGIN = "http://red.hacketo.lan/";


	//std::string WS_LOCATION = "ws://127.0.0.1:9876"; computed

	std::string SERVER_IP = "127.0.0.1";

	short SERVER_PORT = 9876;

	bool USE_SSL = false;

	unsigned int MAX_CLIENTS = 250;


	char DEFAULT_PARSER = protocol::packet::ParserType::JSON;


	bool USE_MODULES = true;
	std::string ACTION_KEY = "action";
	std::string MODULE_KEY = "module";
	std::string MODULE_ACTION_KEY = "action";
	std::string DATA_KEY = "data";


	Config::Config() {

	}

	Config::Config(Config &c) {

	}

	error Config::set(std::string key, std::string value) {
		return error();
	}

	error Config::set(std::string key, int value) {
		return error();
	}

	error Config::set(std::string key, bool value) {
		return error();
	}

	error Config::set(std::string key, float value) {
		return error();
	}
}