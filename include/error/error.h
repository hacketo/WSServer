//
// Created by hacketo on 12/08/16.
//

#ifndef SERVER_ERRORS_H
#define SERVER_ERRORS_H

#include <boost/system/error_code.hpp>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

namespace error{

	class code{
	public:

		uint16_t value;
		std::string msg;
		std::string master;

		code();

		// true if no value
		bool operator!() const BOOST_SYSTEM_NOEXCEPT {
			return value == 0;
		}
		// true if no value
		operator bool () const BOOST_SYSTEM_NOEXCEPT {
			return value != 0;
		}
	};

	std::ostream & operator<<(std::ostream& str, const code& v);

	extern bool handler_init;

	enum _: uint16_t{

		OK = 0,

		SOCKET_CANT_BIND = 10,
		SOCKET_TCP_ACCEPT_ERROR = 11,
		SOCKET_UDP_RECV_ERROR = 12,

		SOCKET_ALREADY_BINDED = 30,
		SOCKET_BUFFER_INCOMPLETE = 31,
		SOCKET_EOF = 32,
		SOCKET_BUFFER_EMPTY = 33,

		/** WS_PROTOCOL */

		HTTP_NO_HEADER = 1000,	// Header envoyé par le client est vide
		HTTP_HEADER_TOO_LONG = 1001,
		HTTP_HEADER_INVALID_KV = 1002,
		HTTP_HEADER_READ_FAILED = 1003,

		WS_HEADER_MISSING_REQUIRED_FIELD = 1004,
		WS_HEADER_WRONG_WS_VERSION = 1005,
		WS_HEADER_WS_KEY_EMPTY = 1006,
		WS_HEADER_WRONG_ORIGIN = 1007,
		WS_HEADER_WRONG_HOST = 1008,

		SEND_ERROR = 1020, // Erreur lors de l'envoi d'un message au client
		RECV_ERROR = 1021, // Erreur lors de l'envoi d'un message au client


		/** Interne */
		SESSION_NEED_UPDATE_COOKIE = 2000,


		/** */
		JSON_DECODE = 3000,

		/** DB 4000 - 4999*/
		DB_CANT_OPEN = 4000,
		SQL_ERROR = 4001,
		DB_NEED_CREATE_TABLE = 4002,

		/** FILE 5000 - 5999*/
		FILE_NOT_FOUND = 5000,




	};

	void get_code(code &c, uint16_t v);
	void get_code(code &c, uint16_t v, const std::string msg);
	void get_code(code &c, uint16_t v, const std::string msg, const boost::system::error_code &ec);
	void get_code(code &c, uint16_t v, const std::string msg, std::string tokenValue);

	void get_code(code &c, const std::string master, uint16_t v, const std::string msg);
	void get_code(code &c, const std::string master, uint16_t v, const std::string msg, const boost::system::error_code &ec);
	void get_code(code &c, const std::string master, uint16_t v, const std::string msg, const std::string tokenValue);

	void init_handler();
}

#endif //SERVER_ERRORS_H
