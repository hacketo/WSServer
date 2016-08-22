//
// Created by hacketo on 10/08/16.
//

#include "statebased_manager.h"
#include "../../debug.h"
#include "../client.h"
#include "../../util/algo.h"

using namespace protocol;

StateBasedManager::StateBasedManager() : Manager(){

	getChangeStatePacket = [](u_int32_t i) -> packet::Packet* {
		return new ChangeStatePacket(i);
	};
}

StateBasedManager::~StateBasedManager(){
	for (std::map<u_int32_t, State*>::iterator it = states.begin(); it != states.end(); ++it){
		delete it->second;
	}
}

void StateBasedManager::initStates(std::vector<State *> list){
	for(std::vector<State *>::iterator it = list.begin(); it != list.end() ; ++it){
		(*it)->init_actions();
		states[ (*it)->id ] = *it;
	}
}

void StateBasedManager::updateClientState(Client *client, u_int32_t id, bool send){
	if (!hasState(id)){
		return;
	}
	State* state = states[client->state_id];
	state->on_exit(client);


	client->state_id = id;
	state = states[id];
	state->on_enter(client);

	if (send){
		packet::Packet *p = getChangeStatePacket(id);
		client->send(p);
		delete p;
	}
}

bool StateBasedManager::hasState(uint32_t state) {
	return algo::map_contains_key<u_int32_t, State*>(&states, state);
}

bool StateBasedManager::onEnter(Client *client) {
	return Manager::onEnter(client);
}

bool StateBasedManager::onReady(Client *client) {
	return Manager::onReady(client);
}

void StateBasedManager::onReceive(Client *client, packet::Packet *packet) {
	if (states.count(client->state_id) > 0){
		State *state = states[client->state_id];


		for(packet::packet_iterator it = packet->begin() ; it != packet->end() ; ++it ){
			state->handle(client, it->get());
		}
	}
}

void StateBasedManager::onClose(Client *client) {
	if (states.count(client->state_id) > 0) {
		State *state = states[client->state_id];

		state->on_exit(client);
	}
}

void StateBasedManager::onError(Client *client) {}
void StateBasedManager::onServerError() {}
void StateBasedManager::onExit() {}


State::State(u_int32_t id, StateBasedManager *manager) : id(id), manager(manager){}
State::~State(){}

void State::init_actions() {}
void State::add_actions(Action *action) {
	actions[action->name] = action;
}
void State::on_enter(Client* client){}
void State::on_exit(Client* client){}

void State::handle(Client *client, packet::PacketData *packet) {
	if (packet->packetType == packet::PacketType::ACTION) {

		packet::ActionPacketData* packetData = reinterpret_cast<packet::ActionPacketData*>(packet);

		if (actions.count(packetData->action) > 0) {
			Action *action = actions[packetData->action];
			action->handle(client, packet->data.get());
		}
	}
}
#ifdef USE_MODULES
ModulesManager* State::getModulesManager(){
	return manager->getModulesManager();
}
#endif

StateBasedManager* State::getManager(){
	return manager;
}


Action::Action(std::string name, State *state) : name(name), state(state){}
void Action::handle(Client* client, GenericValue* packet) {}
void Action::updateClientState(Client *client, u_int32_t id, bool send ){
	state->getManager()->updateClientState(client, id, send);
}