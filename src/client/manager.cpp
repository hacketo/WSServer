//
// Created by hacketo on 10/08/16.
//
#include "client/manager.h"
#include "debug.h"
#include "client/client.h"

Manager::Manager(){
#ifdef USE_MODULES
	modulesManager = new ModulesManager();
#endif
}

bool Manager::onEnter(Client *client){
	DEBUG_PRINT("New client : ",client->get_id(),"-",client->get_ip());
	return true;
}

bool Manager::onHandshakeRecv(Client *client, protocol::http::handshake* handshake){ return true;}
bool Manager::onHandshakeSend(Client *client, protocol::http::handshake* handshake){ return true;}

void Manager::onReady(Client *client){
	DEBUG_PRINT("Start Listening : ",client->get_id(),"-",client->get_ip());
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