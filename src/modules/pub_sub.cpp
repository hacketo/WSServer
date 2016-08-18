//
// Created by hacketo on 14/08/16.
//

#include "pub_sub.h"



ModuleClientController* Pub_sub::getModuleClientController(){
	return new Pub_subClientController(this);
}


bool Pub_sub::reg(Client* client){
	if (!is_client_already_subscribed(client)){
		clients[client->get_id()] = client;
		return true;
	}
	return false;
}

bool Pub_sub::unregister(Client *client){
	if (is_client_already_subscribed(client)){
		clients.erase(client->get_id());
		return true;
	}
	return false;
}


/**
 * Besoin d'etre thread safe
 * @param frameInterface
 * @return
 */
bool Pub_sub::publish(protocol::frame::FrameInterface* frameInterface){
	for(client_iterator it = clients.begin(); it != clients.end(); ++it){
		it->second->send(frameInterface);
	}
	return true;
}

Pub_subClientController::Pub_subClientController(base_module* module) :
		ModuleClientController(module){
}
void Pub_subClientController::handle(std::string action, GenericValue* data) {}