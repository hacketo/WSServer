//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_TCP_H
#define WSSERVERLIB_PROCESSOR_TCP_H

#include "processor.h"

namespace sockets {
	class Tcp;

namespace processor {

	class tcp : public processor {
	public:
		tcp(Tcp *tcp_sock);

		void async_read();


		/**
		 * Call tcp_sock -> read_loop()
		 */
		void read_loop();

	protected:

		virtual void on_read(const boost::system::error_code &ec, std::size_t bytes_read);


		Tcp *m_sock;

	};

}
}

#endif //WSSERVERLIB_PROCESSOR_TCP_H
