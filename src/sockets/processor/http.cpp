//
// Created by hacketo on 03/09/16.
//

#include "sockets/processor/http.h"
#include "sockets/tcp.h"

#include "debug.h"

namespace sockets {
namespace processor {

	http::http(sockets::Tcp *tcp_sock) : tcp(tcp_sock), state(NONE) {}

	void http::on_receive(errors::error_code& ec) {

		if (state == State::NONE) {

			std::string h;
			consume(h);

			headers = new protocol::http::header;
			headers->headers_raw = h;

			protocol::http::parse_header(h, headers, ec);

			if (!ec) {
				state = State::READ_HEADER;
			}
			else{
				DEBUG_PRINT("Erreur du parsage des headers");
				m_sock->close();
			}
		}
	}

}
}