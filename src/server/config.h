//
// Created by hacketo on 12/08/16.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H


#include <string>
#include "../protocol/packet/packet.h"

namespace config {

	extern std::string SERVER_NAME ;// "ServerName"

	extern std::string HTTP_ORIGIN ;// "http://red.hacketo.lan/"


	//std::string WS_LOCATION = "ws://127.0.0.1:9876"; computed

	extern std::string SERVER_IP ;//  "127.0.0.1";

	extern short SERVER_PORT ;//  9876;

	extern bool USE_SSL ;//  false;

	extern unsigned int MAX_CLIENTS ;//  250;


	extern char DEFAULT_PARSER ;//  protocol::packet::ParserType::NO;


	extern bool USE_MODULES ;//  true;
	extern std::string ACTION_KEY ;//  "action";
	extern std::string MODULE_KEY ;//  "module";
	extern std::string MODULE_ACTION_KEY ;//  "action";
	extern std::string DATA_KEY ;//  "data";




	enum ErrorCode {
		NO_PRIVILEGE,
		NEED_REBOOT
	};

	struct error{
		ErrorCode code;
		std::string message;
	};

	class Config {

	public:
		Config();

		Config(Config &c);

		error set(std::string key, std::string value);
		error set(std::string key, int value);
		error set(std::string key, bool value);
		error set(std::string key, float value);
	};
}

#endif //SERVER_CONFIG_H
