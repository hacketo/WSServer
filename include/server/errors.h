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

namespace errors{

	class error_code{
	public:
		uint16_t code;
		std::string msg;
		std::string master;

		error_code();

		// true if no error_code
		bool operator!() const BOOST_SYSTEM_NOEXCEPT {
			return code == 0;
		}
		// true if no error_code
		operator bool () const BOOST_SYSTEM_NOEXCEPT {
			return code != 0;
		}
	};

	std::ostream & operator<<(std::ostream& str, const error_code& v);

	extern bool handler_init;

	enum code: uint16_t{

		OK = 0,

		/** WS_PROTOCOL */

		WS_NO_HEADER = 1000,	// Header envoyé par le client est vide
		WS_HEADER_TOO_LONG = 1001,
		WS_HEADER_INVALID_KV = 1002,
		WS_HEADER_READ_FAILED = 1003,
		WS_HEADER_MISSING_REQUIRED_FIELD = 1004,
		WS_HEADER_WRONG_WS_VERSION = 1005,
		WS_HEADER_WS_KEY_EMPTY = 1006,
		WS_HEADER_WRONG_ORIGIN = 1007,
		WS_HEADER_WRONG_HOST = 1008,

		WS_SEND_ERROR = 1020, // Erreur lors de l'envoi d'un message au client
		WS_RECV_ERROR = 1021, // Erreur lors de l'envoi d'un message au client


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

	error_code get_error(uint16_t code);
	error_code get_error(uint16_t code, std::string msg);
	error_code get_error(uint16_t code, std::string msg, std::string tokenValue);
	error_code get_error(const std::string master, uint16_t code, const std::string msg);
	error_code get_error(const std::string master, uint16_t code, const std::string msg , const std::string tokenValue);



	void init_handler();

}

#endif //SERVER_ERRORS_H
