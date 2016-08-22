//
// Created by hacketo on 28/05/15.
//

#include "protocol/http.h"
#include "util/base64.h"
#include "util/sha1.h"
#include "server/config.h"
#include "util/date.h"


namespace protocol{

namespace http {

	/**
	 * @todo: cacher sa ?
	 * @return
	 */
	std::string get_ws_location(){
		std::stringstream ss;
		ss << "ws";
		if (config::USE_SSL) {
			ss << "s";
		}
		ss << "://" << get_host();
		return ss.str();
	}

	std::string get_host(){
		std::stringstream ss;
		ss << config::SERVER_IP << ":" << config::SERVER_PORT;
		return ss.str();
	}

	http::handshake* get_handshake(const char* unencodedKey) {

		std::string key = unencodedKey + GUID;

		http::handshake* hd = new http::handshake;
		hd->wsKey = base64_encode(get_sha1(key.c_str()));

		hd->baseHeader = "HTTP/1.1 101 Web Socket Protocol Handshake";
		hd->headers["Upgrade"] = "WebSocket";
		hd->headers["Connection"] = "Upgrade";
		hd->headers["Server"] = config::SERVER_NAME;
		hd->headers["Sec-WebSocket-Accept"] = hd->wsKey;
		hd->headers["Sec-WebSocket-Origin"] = config::HTTP_ORIGIN;
		hd->headers["Sec-WebSocket-Location"] = get_ws_location();

		return hd;
	};

	std::string handshake_to_string(http::handshake *pHandshake) {
		std::stringstream ss;

		ss << pHandshake->baseHeader << "\r\n";

		header_iterator it;

		for (it = pHandshake->headers.begin(); it != pHandshake->headers.end(); ++it){
			ss << it->first << ": "<< it->second <<"\r\n";
		}
		for (it = pHandshake->cookies.begin(); it != pHandshake->cookies.end(); ++it){
			ss << "Set-Cookie :" << it->first << "=" << it->second <<"\r\n";
		}
		ss << "\r\n";
		return ss.str();
	}

	void handshake_to_uint8(http::handshake *pHandshake, uint8_t** buffer, uint64_t& size) {
		std::string s = handshake_to_string(pHandshake);
		size = s.length();
		*buffer = new uint8_t[size];
		STRING_TO_UINT8(s,*buffer, size)
	}

	void validate_header(http::handshake *handshake, errors::error& error) {

		http_header &header = handshake->headers;

		if (header.size() == 0) {
			error = errors::get_error(errors::WS_NO_HEADER, "No Header to validate");
			return;
		}

		for (unsigned long i = REQUIRED_FIELDS.size() - 1; i > 0; i--) {
			if (header[REQUIRED_FIELDS[i]].size() == 0) {
				error = errors::get_error(errors::WS_HEADER_MISSING_REQUIRED_FIELD, "Missing field :%s", REQUIRED_FIELDS[i]);
				return;
			}
		}

		if (header[FIELD_WS_VERSION].compare(std::to_string(config::WS_VERSION)) != 0){
			error = errors::get_error(errors::WS_HEADER_WRONG_WS_VERSION, "Wrong ws version :%s", header[FIELD_WS_VERSION]);
			return;
		}
		if (header[FIELD_ORIGIN].compare(config::HTTP_ORIGIN) != 0){
			error = errors::get_error(errors::WS_HEADER_WRONG_ORIGIN, "Wrong origin :%s", header[FIELD_ORIGIN]);
			return;
		}
		if (header[FIELD_HOST].compare(get_host()) != 0){
			error = errors::get_error(errors::WS_HEADER_WRONG_HOST, "Wrong host :%s", header[FIELD_HOST]);
			return;
		}
		if (header[FIELD_WS_KEY].size() == 0){
			error = errors::get_error(errors::WS_HEADER_WS_KEY_EMPTY, "Ws key empty");
			return;
		}

		handshake->wsKey = header[FIELD_WS_KEY];
	}

	void dump_headers(http_header *headers) {
		for (header_iterator iter = headers->begin();
			 iter != headers->end(); iter++) {
			std::cout << "Key: '" << iter->first << "' Value: '" << iter->second << "'" << std::endl;
		}
	}

	void add_cookie(http::handshake* handshake, const std::string& key, const std::string& value, u_int32_t offset){
		std::stringstream ss;
		ss << value;
		if (offset > 0) {
			ss << "; Expires=" << date::utc_from_now(offset);
		}
		handshake->cookies[key] = ss.str();
	}

	void parse_header(const std::string &header_str, http::handshake* handshake, errors::error& error) {

		http_header &headers = handshake->headers;

		if (header_str.length() == 0){
			error = errors::get_error(errors::WS_NO_HEADER, "Header vide");
			return;
		}

		if (header_str.length() > 5000){
			error = errors::get_error(errors::WS_HEADER_TOO_LONG, "Header Trop long");
			return;
		}


		std::vector<std::string> vec;
		boost::iter_split(vec, header_str, boost::algorithm::first_finder("\r\n"));

		std::vector<std::string> linedata;


		boost::iter_split(linedata, vec[0], boost::algorithm::first_finder(" "));

		size_t len = linedata.size();

		if (len != 3){
			error = errors::get_error(errors::WS_HEADER_READ_FAILED, "Header missing HTTP definition");
			return;
		}

		int i = 0;
		for (; i < len; i++) {
			headers[base_header_fields[i]] = linedata[i];
		}

		auto kv_separator_field = boost::algorithm::first_finder(": ");
		auto separator_cookies = boost::algorithm::first_finder("; ");
		auto kv_separator_cookie = boost::algorithm::first_finder("=");
		std::vector<std::string> linedataCookies;
		std::vector<std::string> lineValueCookies;

		for (i = 1, len = vec.size(); i < len; i++) {
			if (vec[i].size() > 0) {
				boost::iter_split(linedata, vec[i], kv_separator_field);
				if (linedata.size() == 2) {

					if (linedata[0].compare("Cookie") == 0){
						boost::iter_split(linedataCookies, linedata[1], separator_cookies);
						for (int y = 0, len2 = linedataCookies.size(); y < len2; y++) {
							boost::iter_split(lineValueCookies, linedataCookies[y], kv_separator_cookie);
							if (lineValueCookies.size() == 2) {
								handshake->cookies[lineValueCookies[0]] = lineValueCookies[1];
							}
						}

						continue;
					}

					headers[linedata[0]] = linedata[1];
				}
				else{
					error = errors::get_error(errors::WS_HEADER_INVALID_KV, "Invalid key/value :%s", vec[i]);
					return;
				}
			}
		}
	}
}
}
