//
// Created by hacketo on 01/09/16.
//

#ifndef WSSERVERLIB_SOCKET_H
#define WSSERVERLIB_SOCKET_H

#include <boost/asio.hpp>


#include "error/error.h"
#include "protocol/constant.h"
#include "processor/processor.h"

class Client;

using namespace boost;

namespace sockets {

	/**
	 * Abstract Class that represent a Client Socket
	 * @see sockets::processor
	 */
	class Socket {
		friend class processor::processor;
	public:

		typedef std::unique_ptr<Socket> u_ptr;

		Socket();

		std::string get_ip();

		virtual size_t send(std::string &message, error::code& ec) = 0;

		template <typename BufferSequence>
		size_t send(BufferSequence& buffer, size_t size, error::code &ec){}

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
		void bind_client(Client *c, error::code& ec);

		/**
		 * Attach a processor to that socket
		 * ec can be equals to errors::SOCKET_ALREADY_BINDED
		 * @param c
		 * @param ec
		 */
		void bind_processor(processor::processor *p, error::code& ec);

		/**
		 * Start the socket job , read and write
		 * @return
		 */
		virtual void start(error::code& ec);

		/**
		 * Get the client pointer
		 * @return
		 */
		Client* client();

	protected:
		/**
		 * Write data to the socket, should only be called by processor::send
		 * @param buffer
		 * @param ec
		 * @return
		 */
		virtual size_t write(const asio::const_buffers_1& buffers, error::code &ec) = 0;


		Client* m_client;

		std::string m_ip;

		std::atomic<bool> m_closed;

		std::atomic<bool> m_clientBinded;
		std::atomic<bool> m_procBinded;

		processor::processor* m_processor;
	};

}

#endif //WSSERVERLIB_SOCKET_H
