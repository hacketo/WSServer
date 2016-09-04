//
// Created by hacketo on 15/08/16.
//

#include <sstream>
#include "error/error.h"
#include "util/algo.h"

namespace error {

	code::code() {
		value = OK;
		msg = "";
		master = "";
	}

	std::ostream &operator<<(std::ostream &str, const code &v) {
		if (v.master.length() > 0) {
			str << "[" << v.master << "] : ";
		}
		str << "(" << v.value << ") : " << v.msg;
		return str;
	}

	void get_code(code &c, uint16_t v) {
		get_code(c, v, "");
	};

	void get_code(code &c, uint16_t v, const std::string msg, const boost::system::error_code &ec) {
		get_code(c, "", v, msg, ec);
	};

	void get_code(code &c, uint16_t v, const std::string msg) {
		get_code(c, "", v, msg);
	};

	void get_code(code &c, uint16_t v, const std::string msg, std::string tokenValue) {
		get_code(c, "", v, msg, tokenValue);
	};


	void get_code(code &c, const std::string master, uint16_t v, const std::string msg) {
		c.value = v;
		c.msg = msg;
		c.master = master;
	};

	void get_code(code &c, const std::string master, uint16_t v, const std::string msg,
				  const boost::system::error_code &ec) {
		std::stringstream ss;
		ss << msg << " : [" << ec.value() << "] "<< ec.message();
		get_code(c, master, v, ss.str());
	};

	void get_code(code &c, const std::string master, uint16_t v, const std::string msg, const std::string tokenValue) {
		get_code(c, master, v, msg);
		SPRINTF(c.msg, tokenValue);
	};

	bool handler_init = false;

	void init_handler() {
		if (!handler_init) {
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