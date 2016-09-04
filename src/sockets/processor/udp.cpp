//
// Created by hacketo on 03/09/16.
//

#include "sockets/processor/udp.h"
#include "sockets/udp.h"

namespace sockets {
namespace processor {


	udp::udp(Udp* sock): processor(sock){

	}

	void udp::on_read(const boost::system::error_code &e, std::size_t bytes_read){
		processor::on_read(e, bytes_read);
		if(e){
			if (e.value() == boost::asio::error::eof) {
				m_sock->close();
			}
			DEBUG_PRINT("Error while recv : ", e.message());
		}
		else{
			read_loop();
		}
	}
}
}