//
// Created by hacketo on 01/09/16.
//

#include <protocol/constant.h>
#include <boost/asio.hpp>
#include <debug.h>
#include <boost/bind.hpp>
#include <protocol/packet.h>
#include "sockets/processor.h"
#include "client/client.h"

namespace sockets {

	processor::processor(TcpSocket *tcp_sock) : tcp_sock(tcp_sock) {
	}

	void processor::process() {

		asio::streambuf::mutable_buffers_type mutableBuffer = m_buffer.prepare(max_buffer_len);
		asio::async_read(tcp_sock->sock(), mutableBuffer, asio::transfer_at_least(1),
						 boost::bind(&processor::on_read, this, _1, _2));
	}

	void processor::consume(std::string& s){
		boost::asio::streambuf::const_buffers_type bufs = m_buffer.data();
		s = std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_end(bufs));
		m_buffer.consume(m_buffer_size);
		m_buffer_size = 0;
	}

	void processor::consume(char *s, size_t& size){
		s = new char[m_buffer_size];
		size = m_buffer_size;
		boost::asio::streambuf::const_buffers_type bufs = m_buffer.data();
		std::copy(boost::asio::buffers_begin(bufs), boost::asio::buffers_end(bufs), s);
		m_buffer.consume(m_buffer_size);
		m_buffer_size = 0;
	}

	void processor::on_receive() {
//		char *buffer;
//		size_t size;
//		consume(buffer,size);

		std::string msg;
		consume(msg);

		errors::error_code ec2;
		protocol::packet::Packet::u_ptr p = protocol::packet::Packet::u_ptr(new protocol::packet::Packet());
		protocol::packet::parse(p.get(), msg, ec2);

		if (!ec2) {
			tcp_sock->m_client->on_receive(p.get());
		} else {
			DEBUG_PRINT("error while parsing the response");
		}
		DEBUG_PRINT("recv [", msg.size(), "] : ", msg);
	}

	void processor::on_read(const boost::system::error_code &e, std::size_t bytes_read) {

		if (!e) {
			if (bytes_read > 0) {
				m_buffer_size += bytes_read;
				m_buffer.commit(bytes_read);
				if (bytes_read < max_buffer_len) {
					on_receive();
				} else {
					DEBUG_PRINT("recv increasing buffer !!");
				}
			} else {
				DEBUG_PRINT("recv NOTHING !! : ");
			}
			read_loop();
		} else {
			if (e.value() == boost::asio::error::eof) {
				tcp_sock->close();
			}
			DEBUG_PRINT("Error while recv : ", e.message());
		}
	}

	void processor::read_loop() {
		tcp_sock->read_loop();
	}

	void processor::send(char *buffer, size_t size) {
		tcp_sock->sock().send(asio::buffer(buffer, size));
	}


	httpprocessor::httpprocessor(sockets::TcpSocket *tcp_sock) : processor(tcp_sock), state(NONE) {}

	void httpprocessor::on_receive() {

		if (state == State::NONE) {
			consume(headers);

			DEBUG_PRINT("receive_headers");

			state = State::READ_HEADER;
		}
	}


	wsprocessor::wsprocessor(sockets::TcpSocket *tcp_sock) : httpprocessor(tcp_sock) {}

	void wsprocessor::on_receive() {

		if (state == State::NONE) {
			// can change the state
			httpprocessor::on_receive();

			if (state == State::READ_HEADER){

				DEBUG_PRINT("parse_and_send_response_headers", headers);

			}
		}


	}

}
