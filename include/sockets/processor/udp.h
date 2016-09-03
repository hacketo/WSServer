//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_UDP_H
#define WSSERVERLIB_PROCESSOR_UDP_H

#include "processor.h"


namespace sockets {
	class Udp;

namespace processor {

	class udp : public processor<Udp>{
	public:
		udp(Udp *tcp_sock);
	};

}
}


#endif //WSSERVERLIB_PROCESSOR_UDP_H
