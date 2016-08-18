//
// Created by hacketo on 28/05/15.
//

#include "http.h"
#include "../util/base64.h"
#include "../util/sha1.h"
#include <sstream>
namespace protocol{

namespace http {

	handshake get_handshake(std::string server_name, std::string unencodedKey,
							  std::string http_origin, std::string ws_location) {

		std::string key = unencodedKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		std::string ws_key = base64_encode(get_sha1(key.c_str()));

		std::stringstream ss;
		ss << "HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
		   << "Upgrade: WebSocket\r\n"
		   << "Connection: Upgrade\r\n"
		   << "Server: " << server_name << "\r\n"
		   << "Sec-WebSocket-Accept: " << ws_key << "\r\n"
		   << "Sec-WebSocket-Origin: " << http_origin << "\r\n"
		   << "Sec-WebSocket-Location: " << ws_location << "\r\n\r\n";

		handshake h;
		h.header = ss.str();
		h.wsKey = ws_key;

		return h;
	};


	bool validate_header(http_header &header) {
		if (header.size() == 0) {
			return false;
		}

		for (unsigned long i = REQUIRED_FIELDS.size() - 1; i > 0; i--) {
			if (header[REQUIRED_FIELDS[i]].size() == 0) {
				return false;
			}
		}
		return true;
	}

	void dump_headers(http_header *headers) {
		for (header_iterator iter = headers->begin();
			 iter != headers->end(); iter++) {
			std::cout << "Key: '" << iter->first << "' Value: '" << iter->second << "'" << std::endl;
		}
	}

	http_header parse_header(const std::string &header, http_header &headers) {

		std::vector<std::string> vec;
		boost::iter_split(vec, header, boost::algorithm::first_finder("\r\n"));

		std::vector<std::string> linedata;
		boost::iter_split(linedata, vec[0], boost::algorithm::first_finder(" "));

		size_t len = linedata.size();
		int i = 0;
		for (; i < len; i++) {
			headers[base_header_fields[i]] = linedata[i];
		}

		auto finder = boost::algorithm::first_finder(": ");

		for (i = 1, len = vec.size(); i < len; i++) {
			if (vec[i].size() > 0) {
				boost::iter_split(linedata, vec[i], finder);
				headers[linedata[0]] = linedata[1];
			}
		}

		return headers;
	}
}
}
