//
// Created by hacketo on 15/08/16.
//

#include "server/errors.h"
#include "util/algo.h"

namespace errors {

error_code::error_code() {
	code = 0;
	msg = "";
	master = "";
};


std::ostream &operator<<(std::ostream &str, const error_code &v) {
	if (v.master.length() > 0) {
		str << "[" << v.master << "] : ";
	}
	str << "(" << v.code << ") : " << v.msg;
	return str;
}

error_code get_error(uint16_t code) {
	error_code e;
	e.code = code;
	return e;
};

error_code get_error(uint16_t code, const std::string msg) {
	error_code e;
	e.code = code;
	e.msg = msg;
	return e;
};

error_code get_error(uint16_t code, const std::string msg, const std::string tokenValue) {
	error_code e;
	e.code = code;
	e.msg = msg;
	SPRINTF(e.msg ,tokenValue);
	return e;
};

error_code get_error(const std::string master, uint16_t code, const std::string msg) {
	error_code e;
	e.code = code;
	e.msg = msg;
	e.master = master;
	return e;
};

error_code get_error(const std::string master, uint16_t code, const std::string msg, const std::string tokenValue) {
	error_code e;
	e.code = code;
	e.msg = msg;
	SPRINTF(e.msg ,tokenValue);
	e.master = master;
	return e;
};

bool handler_init = false;

void init_handler(){
	if(!handler_init) {
		handler_init = true;
		signal(SIGSEGV, [](int sig) {
			void *array[10];
			size_t size;

			// get void*'s for all entries on the stack
			size = backtrace(array, 10);

			// print out all the frames to stderr
			fprintf(stderr, "Error: signal %d:\n", sig);
			backtrace_symbols_fd(array, size, STDERR_FILENO);
			exit(1);
		});
	}
}

}