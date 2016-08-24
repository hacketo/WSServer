//
// Created by hacketo on 14/08/16.
//

#include <debug.h>
#include "ext/modules/pub_sub.h"
#include "client/client.h"


namespace ext {
namespace modules {
namespace pubsub {


	OutgoingMessagesWorker::OutgoingMessagesWorker(Pub_sub *pubsub, size_t size) :
			WorkerDeQue<Frame*>(size), pubsub(pubsub){
	}

	void OutgoingMessagesWorker::do_job(Frame* frame){
		frame::encode(frame);

		assert(frame->encoded);

		for (base_module::client_iterator it = pubsub->begin(); it != pubsub->end(); ++it) {
			if (it->second) {
				it->second->send(frame);
			}
		}
	}


	Pub_sub::Pub_sub() : base_module(){
		worker = OutgoingMessagesWorker::create(this);
		worker->init_job_thread();
	}

	/**
	 * Besoin d'etre thread safe
	 * @param frameInterface
	 * @return
	 */
	bool Pub_sub::publish(protocol::frame::FrameInterface *frameInterface) {
		DEBUG_PRINT("Pub_sub::worker->dispatch(frameInterface->getFrame())");
		return worker->dispatch(frameInterface->getFrame());
	}

	ModuleClientController *Pub_sub::getNewClientController() {
		return new Pub_subClientController(this);
	}

	errors::error_code Pub_sub::join(){
		alive = false;
		worker->join(true);

		return errors::error_code();
	}


	Pub_subClientController::Pub_subClientController(base_module *module) :
			ModuleClientController(module) {
	}
	void Pub_subClientController::handle(Client *client, std::string& action, GenericValue *data) {}

}
}
}