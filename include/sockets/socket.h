//
// Created by hacketo on 01/09/16.
//

#ifndef WSSERVERLIB_SOCKET_H
#define WSSERVERLIB_SOCKET_H

#include <boost/asio.hpp>


#include "server/errors.h"
#include "protocol/constant.h"
#include "processor/processor.h"

class Client;

using namespace boost;

namespace sockets {

	class Socket {

	public:

		typedef std::unique_ptr<Socket> u_ptr;

		Socket();


		std::string get_ip();


		virtual size_t send(std::string &message, errors::error_code& ec) = 0;

		template <typename BufferSequence>
		size_t send(BufferSequence& buffer, size_t size, errors::error_code &ec){}

		/**
		 * Close the socket
		 * Then not able to read or write
		 */
		virtual void close() = 0;

		/**
		 * @return true if the socket is closed
		 */
		bool closed() const;

		/**
		 * Attach a client to that socket
		 * ec can be equals to errors::SOCKET_ALREADY_BINDED
		 * @param c
		 * @param ec
		 */
		void bind_client(Client *c, errors::error_code& ec);

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(errors::error_code& ec);

		/**
		 * Write data to the socket, should only be called by processor::send
		 * @param buffer
		 * @param ec
		 * @return
		 */
		virtual size_t write(const asio::mutable_buffer& buffers, errors::error_code &ec) = 0;


		/**
		 * Get the client pointer
		 * @return
		 */
		Client* client();

	protected:
		Client* m_client;

		std::string m_ip;
		std::atomic<bool> m_closed;
		std::atomic<bool> m_clientBinded;
	};

}

#endif //WSSERVERLIB_SOCKET_H
