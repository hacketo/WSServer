//
// Created by hacketo on 28/05/15.
//

#ifndef WS_SERVER_STRING_H
#define WS_SERVER_STRING_H

#include <string>
#include <sstream>
#include <vector>

namespace string {
	inline void convert(const std::string &s, unsigned char *ret, size_t max_size = 0) {
		const char *string = s.c_str();
		size_t offset = max_size > 0 ? std::min<size_t >(s.size(),max_size) : s.size();
		std::copy(string, string + offset, ret);
	}

	inline std::string sprintf(const std::string& msg , const std::string& tokenValue) {
		uint64_t size = msg.length() + tokenValue.length();
		char buffer[size];
		std::sprintf(buffer, msg.c_str(), tokenValue);
		return std::string(buffer);
	};
}

#endif //WS_SERVER_STRING_H
