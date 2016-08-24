//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_PUB_SUB_H
#define SERVER_PUB_SUB_H


#include <stdint.h>
#include "base_module.h"
#include <memory>
#include <util/worker.h>

using protocol::frame::Frame;

class Client;

namespace ext {
namespace modules {
namespace pubsub {
	class Pub_sub;

	class OutgoingMessagesWorker : public WorkerDeQue<Frame*> {
	public:
		typedef std::unique_ptr<OutgoingMessagesWorker> u_ptr;

		static u_ptr create(Pub_sub *pubsub, size_t size = 50) {
			return u_ptr(new OutgoingMessagesWorker(pubsub, size));
		}

		OutgoingMessagesWorker(Pub_sub *pubsub, size_t size = 50);

		void do_job(Frame * f) override;

	private:
		Pub_sub *pubsub;


	};

	class Pub_sub : public base_module {

		friend class OutgoingMessagesWorker;
		friend class Pub_subModule;

	public:

		Pub_sub();

		typedef std::unique_ptr<Pub_sub> u_ptr;


		/**
		 * Appelé pour publier un message auc clients abonnés
		 * @THREAD1
		 * @param packet
		 * @return
		 */
#pragma warning disable 0169
		bool publish(protocol::frame::FrameInterface *frameInterface);
#pragma warning restore 0169
	protected:
		OutgoingMessagesWorker::u_ptr worker;

		virtual errors::error_code join() override;
		virtual ModuleClientController *getNewClientController() override;

	};


	class Pub_subClientController : public ModuleClientController {
	public:
		Pub_subClientController(base_module *module);

		virtual void handle(Client *client, std::string& action, GenericValue *data);
	};

}
}
}
#endif //SERVER_PUB_SUB_H
