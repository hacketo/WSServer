//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_WS_H
#define WSSERVERLIB_PROCESSOR_WS_H


#include "http.h"

namespace sockets {
namespace processor {

	class websocket : public http {

	public:
		websocket(Tcp *tcp_sock);

	protected:
		/**
		 * Called when we have data from client
		 */
		virtual void on_receive(errors::error_code& ec);


		/**
		 * @param msg
		 * @param ec
		 * @return
		 */
		virtual size_t send(std::string& msg, errors::error_code &ec);

	private:
		void process_http_handshake();
		void process_frame();
	};

}
}


#endif //WSSERVERLIB_PROCESSOR_WS_H
