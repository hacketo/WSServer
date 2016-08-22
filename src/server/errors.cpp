//
// Created by hacketo on 15/08/16.
//

#include "server/errors.h"
#include "util/algo.h"

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

error get_error(uint16_t code, const std::string msg) {
	error e;
	e.code = code;
	e.msg = msg;
	return e;
};

error get_error(uint16_t code, const std::string msg, const std::string tokenValue) {
	error e;
	e.code = code;
	e.msg = msg;
	SPRINTF(e.msg ,tokenValue);
	return e;
};

error get_error(const std::string master, uint16_t code, const std::string msg) {
	error e;
	e.code = code;
	e.msg = msg;
	e.master = master;
	return e;
};

error get_error(const std::string master, uint16_t code, const std::string msg, const std::string tokenValue) {
	error e;
	e.code = code;
	e.msg = msg;
	SPRINTF(e.msg ,tokenValue);
	e.master = master;
	return e;
};

}