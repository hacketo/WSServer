//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_TCP_H
#define WSSERVERLIB_PROCESSOR_TCP_H

#include "processor.h"

namespace sockets {
	class Tcp;

namespace processor {

	class tcp : public processor<Tcp> {
	public:
		tcp(Tcp *tcp_sock);

		void process();

		/**
		 * Call tcp_sock -> read_loop()
		 */
		void read_loop();
	};

}
}

#endif //WSSERVERLIB_PROCESSOR_TCP_H
