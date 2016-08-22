//
// Created by hacketo on 15/08/16.
//

#include "server/errors.h"
#include "util/string.h"

namespace errors {

error::error() {
	code = 0;
	msg = "";
	master = "";
};


std::ostream &operator<<(std::ostream &str, const error &v) {
	if (v.master.length() > 0) {
		str << "[" << v.master << "] : ";
	}
	str << "(" << v.code << ") : " << v.msg;
	return str;
}

error get_error(uint16_t code) {
	error e;
	e.code = code;
	return e;
};

error get_error(uint16_t code, std::string msg) {
	error e;
	e.code = code;
	e.msg = msg;
	return e;
};

error get_error(uint16_t code, std::string msg, std::string tokenValue) {
	error e;
	e.code = code;
	e.msg = string::sprintf(msg, tokenValue);
	return e;
};

error get_error(std::string master, uint16_t code, std::string msg) {
	error e;
	e.code = code;
	e.msg = msg;
	e.master = master;
	return e;
};

error get_error(std::string master, uint16_t code, std::string msg, std::string tokenValue) {
	error e;
	e.code = code;
	e.msg = string::sprintf(msg, tokenValue);
	e.master = master;
	return e;
};

}