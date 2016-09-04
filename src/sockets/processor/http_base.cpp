//
// Created by hacketo on 03/09/16.
//

#include "sockets/processor/http_base.h"
#include "sockets/tcp.h"

#include "debug.h"

namespace sockets {
namespace processor {

	http_base::http_base(Tcp *tcp_sock) : tcp(tcp_sock), state(NONE) {}

	http_base::~http_base(){
		delete headers;
	}

	void http_base::on_receive(error::code& ec) {

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