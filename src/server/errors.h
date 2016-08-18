//
// Created by hacketo on 12/08/16.
//

#ifndef SERVER_ERRORS_H
#define SERVER_ERRORS_H

#include <boost/system/error_code.hpp>

namespace errors{

	class error{
	public:
		uint16_t code;
		std::string msg;
		std::string master;
		error();
		bool operator!() const  BOOST_SYSTEM_NOEXCEPT // true if no error
		{
			return code == 0;
		}
	};

	enum code: uint16_t{

		OK = 0,



		JSON_DECODE = 3000,




	};

	error get_error(uint16_t code, std::string msg);
	error get_error(std::string master, uint16_t code, std::string msg);
}

#endif //SERVER_ERRORS_H
