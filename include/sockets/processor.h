//
// Created by hacketo on 01/09/16.
//

#ifndef WSSERVERLIB_PROCESSOR_H
#define WSSERVERLIB_PROCESSOR_H


#include <server/errors.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include "socket.h"


using namespace boost;

namespace sockets {

	class processor {
	public:
		processor(TcpSocket *tcp_sock);

		void process();

		void send(char *buffer, size_t size);

	protected:

		/**
		 * Called when we have data from client
		 */
		virtual void on_receive();

		void consume(std::string& s);
		void consume(char *s, size_t& size);

		TcpSocket *tcp_sock;
		boost::asio::streambuf m_buffer;
		size_t m_buffer_size;

		size_t max_buffer_len = protocol::constant::max_buffer_size;

	private:
		/**
		 * Call tcp_sock -> read_loop()
		 */
		void read_loop();

		void on_read(const boost::system::error_code &ec, std::size_t bytes_read);
	};

	class httpprocessor : public processor {

	public:
		enum State {
			NONE, READ_HEADER
		};

		httpprocessor(TcpSocket *tcp_sock);

	protected:

		std::string headers;
		/**
		 * Called when we have data from client
		 */
		virtual void on_receive();

		char state;
	};

	class wsprocessor : public httpprocessor {

	public:
		wsprocessor(TcpSocket *tcp_sock);

	protected:
		/**
		 * Called when we have data from client
		 */
		virtual void on_receive();
	};


}

#endif //WSSERVERLIB_PROCESSOR_H
