//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_WS_H
#define WSSERVERLIB_PROCESSOR_WS_H


#include "http_base.h"

namespace sockets {
namespace processor {

	class websocket : public http_base {

	public:
		websocket(Tcp *tcp_sock);

	protected:
		/**
		 * Called when we have data from client
		 */
		virtual void on_receive(error::code& ec);


		/**
		 * @param msg
		 * @param ec
		 * @return
		 */
		virtual size_t send(std::string& msg, error::code &ec);

	private:
		void process_http_handshake();
		void process_frame();
	};

}
}


#endif //WSSERVERLIB_PROCESSOR_WS_H
