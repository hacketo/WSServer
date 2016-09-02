//
// Created by hacketo on 01/09/16.
//

#include <protocol/constant.h>
#include <boost/asio.hpp>
#include <debug.h>
#include "protocol/processor.h"
#include <server/socket.h>

using namespace sockets;

processor::processor(){
	reset_buffer();
}

void processor::reset_buffer(){
	buffer.reset(new boost::asio::streambuf);
	size_ = 0;
}

tcp_processor::tcp_processor(sockets::TcpSocket *tcp_sock) : processor(),
		tcp_sock(tcp_sock), sock_(tcp_sock->sock()){

}
void tcp_processor::process(){

	asio::streambuf::mutable_buffers_type buf = buffer.get()->prepare(400);

	sock_->async_read_some(buf,
		[this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
			size_ = bytes_transferred;
			if (!ec){

				DEBUG_PRINT("recv : " );

				read_loop();
			}
			else{

			}
	});

}

void tcp_processor::read_loop(){
	tcp_sock->read_loop();
}



httpprocessor::httpprocessor(sockets::TcpSocket *tcp_sock) : tcp_processor(tcp_sock), state(NONE){}


void httpprocessor::process(){

	reset_buffer();

	if (state == State::NONE) {
		asio::async_read_until(sock_, *buffer.get(), "\r\n",
		   [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
			   size_ = bytes_transferred;
			   if (!ec){
				   state = READ_REQUEST;
				   read_loop();
			   }
			   else{

			   }
	   });
	}
	if (state == State::READ_REQUEST){
		asio::async_read_until(sock_, *buffer.get(), "\r\n\r\n",
		   [this](const boost::system::error_code& ec,std::size_t bytes_transferred){
			   size_ = bytes_transferred;
			   if (!ec){
				   state = READ_HEADER;
				   read_loop();
			   }
			   else{
			   }
		 });
	}

	if (state == State::READ_HEADER){
		state = NONE;
		read_loop();
	}
}

