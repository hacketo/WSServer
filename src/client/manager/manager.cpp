//
// Created by hacketo on 10/08/16.
//
#include "manager.h"
#include "../../debug.h"
#include "../client.h"

Manager::Manager(){
#ifdef USE_MODULES
	modulesManager = new ModulesManager();
#endif
}

bool Manager::onEnter(Client *client){
	debug::print("New client : ",client->get_id(),"-",client->get_ip());
	return true;
}

bool Manager::onHandshakeRecv(Client *client, protocol::http::handshake* handshake){ return true;}
bool Manager::onHandshakeSend(Client *client, protocol::http::handshake* handshake){ return true;}

bool Manager::onReady(Client *client){
	debug::print("Start Listening : ",client->get_id(),"-",client->get_ip());
	return true;
}

void Manager::onReceive(Client *client, protocol::packet::Packet *packet){}
void Manager::onClose(Client *client){}
void Manager::onError(Client *client){}
void Manager::onServerError(){}
void Manager::onExit(){}

#ifdef USE_MODULES
ModulesManager* Manager::getModulesManager(){
	return modulesManager;
}
#endif