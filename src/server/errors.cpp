//
// Created by hacketo on 15/08/16.
//

#include "errors.h"
#include "../util/string.h"

errors::error::error(){
	code = 0;
	msg = "";
	master = "";
};

errors::error errors::get_error(uint16_t code) {
	error e;
	e.code = code;
	return e;
};

errors::error errors::get_error(uint16_t code, std::string msg) {
	error e;
	e.code = code;
	e.msg = msg;
	return e;
};
errors::error errors::get_error(uint16_t code, std::string msg, std::string tokenValue) {
	error e;
	e.code = code;
	e.msg = string::sprintf(msg, tokenValue);
	return e;
};

errors::error errors::get_error(std::string master, uint16_t code, std::string msg) {
	error e;
	e.code = code;
	e.msg = msg;
	e.master = master;
	return e;
};

errors::error errors::get_error(std::string master, uint16_t code, std::string msg , std::string tokenValue) {
	error e;
	e.code = code;
	e.msg = string::sprintf(msg, tokenValue);
	e.master = master;
	return e;
};

