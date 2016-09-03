//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_HTTP_H
#define WSSERVERLIB_PROCESSOR_HTTP_H


#include "tcp.h"
#include "protocol/http.h"

namespace sockets {
namespace processor {
	class http : public tcp {

	public:
		enum State {
			NONE, READ_HEADER
		};

		http(Tcp *tcp_sock);

	protected:

		protocol::http::header* headers;
		/**
		 * Called when we have data from client
		 */
		virtual void on_receive(errors::error_code& ec);

		char state;
	};

}
}


#endif //WSSERVERLIB_PROCESSOR_HTTP_H
