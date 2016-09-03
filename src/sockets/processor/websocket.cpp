//
// Created by hacketo on 03/09/16.
//

#include "sockets/processor/websocket.h"
#include "sockets/tcp.h"

#include "protocol/frame.h"
#include "protocol/opcode.h"
#include "protocol/packet.h"

#include "debug.h"

namespace sockets {
namespace processor {

	websocket::websocket(Tcp *sock) : http(sock) {}

	void websocket::on_receive(errors::error_code& _) {

		if (state == State::NONE) {
			process_http_handshake();
		}
		else{
			process_frame();
		}
	}

	void websocket::process_frame(){
		protocol::frame::Frame::u_ptr frame = protocol::frame::Frame::create();

		size_t bufferSize = m_buffer_size;
		uint8_t buffer[bufferSize];
		consume<uint8_t>(buffer);


		// Todo: check size, frame::get_framelen(&frameBuffer);
		protocol::frame::decode(frame.get(), buffer, bufferSize);
		if (frame->op_code == protocol::opcode::CLOSE) {
			m_sock->close();
			return;
		}

		errors::error_code e;
		protocol::packet::Packet::u_ptr p = protocol::packet::Packet::u_ptr(new protocol::packet::Packet());
		protocol::packet::parse(p.get(), frame->msg, e);

		m_sock->client()->on_receive(p.get());

		delete buffer;
	}

	size_t websocket::send(std::string& msg, errors::error_code &ec){
		protocol::frame::Frame* f = protocol::frame::from_string(msg);
		size_t bytes_sent  = processor::send(f->buffer, f->bufferSize, ec);
		delete f;
		return bytes_sent;
	}

	void websocket::process_http_handshake(){
		errors::error_code ec;

		// can change the state
		http::on_receive(ec);

		// Si error lors de réception du header http
		if (ec){
			DEBUG_PRINT("Erreur lors de la réception du header");
			m_sock->close();
			return;
		}

		// Vérification de l'état, normalement devrait toujours être vrai
		if (state == State::READ_HEADER){

			DEBUG_PRINT("parse_and_send_response_headers", headers->baseHeader);

			protocol::http::validate_header(headers, ec);

			if (ec) {
				DEBUG_PRINT("Erreur lors de la validation du header");
				m_sock->close();
				return;
			}

			bool c_processing = m_sock->client()->on_handshakerecv(headers, ec);
			// if header has cookie sid, récupérer la session

			if (ec || !c_processing){
				DEBUG_PRINT("Header non accepté par le manager");
				m_sock->close();
				return;
			}


			protocol::http::header* handshake = protocol::http::get_handshake(headers->wsKey.c_str());

			c_processing = m_sock->client()->on_handshakesend(handshake, ec);

			if (!ec && c_processing){

				std::string s = protocol::http::handshake_to_string(handshake);

				size_t bytes_sent = send(s, ec);
				if (!ec && bytes_sent == s.length()){
					m_sock->client()->on_ready();
				}
				else{
					DEBUG_PRINT("Error while sending handshake");
					m_sock->close();
				}
			}
			else{
				DEBUG_PRINT("Header non accepté par le manager");
				m_sock->close();
			}

			delete handshake;
		}
	}

}
}