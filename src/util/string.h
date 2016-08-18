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
}

#endif //WS_SERVER_STRING_H
