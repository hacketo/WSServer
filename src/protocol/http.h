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


namespace protocol{
namespace http {

	struct handshake{
		std::string header;
		std::string wsKey;
	};

	typedef std::map<std::string, std::string> http_header;

	typedef http_header::const_iterator header_iterator;

	const std::vector<std::string> REQUIRED_FIELDS(
			{"Sec-WebSocket-Version", "Sec-WebSocket-Key", "Host", "Origin"}
	);

	const std::string base_header_fields[]{"Method", "Request-URI", "HTTP-Version"};


	handshake get_handshake(std::string server_name, std::string unencodedkey, std::string http_origin, std::string ws_location);

	/**
	 * Valide le header
	 * @param header
	 * @return True sir le header est valide
	 */
	bool validate_header(http_header &header);

	/**
	 * Utiliser pour débugger les header
	 * @param headers
	 */
	void dump_headers(http_header *headers);


	http_header parse_header(const std::string &header, http_header &headers);

	http_header parseMap(const char *header, const size_t header_size);

	bool next_chars_CRLF(const char *header, int i);
}
}
#endif //WS_SERVER_HTTP_H
