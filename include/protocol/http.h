//
// Created by hacketo on 28/05/15.
//

#ifndef WS_SERVER_HTTP_H
#define WS_SERVER_HTTP_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/finder.hpp>
#include "server/errors.h"


namespace protocol{
namespace http {

	typedef std::map<std::string, std::string> http_header;

	struct header{
		std::string baseHeader;
		std::string headers_raw;
		http_header headers;
		http_header cookies;
		std::string wsKey;
	};



	typedef http_header::const_iterator header_iterator;


	const std::string GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	const std::string FIELD_WS_VERSION = "Sec-WebSocket-Version";
	const std::string FIELD_WS_KEY = "Sec-WebSocket-Key";
	const std::string FIELD_HOST = "Host";
	const std::string FIELD_ORIGIN = "Origin";

	const std::vector<std::string> REQUIRED_FIELDS(
			{FIELD_WS_VERSION, FIELD_WS_KEY, FIELD_HOST, FIELD_ORIGIN}
	);

	const std::string base_header_fields[]{"Method", "Request-URI", "HTTP-Version"};

	std::string get_ws_location();
	std::string get_host();

	http::header* get_handshake(const char * unencodedkey);

	void add_cookie(http::header* handshake, const std::string& key, const std::string& value, u_int32_t offset);
	void parse_header(const std::string& header, http::header* handshake, errors::error_code& error);

	/**
	 * Valide le header
	 * @param header
	 * @return True sir le header est valide
	 */
	void validate_header(http::header* handshake, errors::error_code& error);

	/**
	 * Utiliser pour débugger les header
	 * @param headers
	 */
	void dump_headers(http_header *headers);




	http_header parseMap(const char *header, const size_t header_size);

	bool next_chars_CRLF(const char *header, int i);

	std::string handshake_to_string(header *pHandshake);
	void handshake_to_uint8(http::header *pHandshake, uint8_t** buffer, uint64_t& size);
}
}
#endif //WS_SERVER_HTTP_H
