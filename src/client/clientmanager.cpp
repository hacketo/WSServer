//
// Created by hacketo on 03/09/16.
//

#include "client/clientmanager.h"



ClosingClientsWorker::ClosingClientsWorker(ClientManager* manager, size_t size) :
		WorkerDeQue<u_int32_t>(size), manager(manager){
	debug_tag = "ClosingClientsWorker";
}

void ClosingClientsWorker::do_job(u_int32_t clientId){
	manager->handleClientClosed(clientId);
}

ClientManager::ClientManager(Manager* m) :
		Client_ID(0), manager(Manager::u_ptr(m)) {

	alive.store(true);

}

void ClientManager::init(){
#ifdef USE_MODULES
	modulesManager = manager->getModulesManager();
#endif

	worker_closingclients = ClosingClientsWorker::u_ptr(new ClosingClientsWorker(this));
	worker_closingclients->init_job_thread();

#ifdef USE_SESSIONS
	sessionManager = SessionManager::u_ptr(new SessionManager);
	alive = sessionManager->alive;
#endif
}

void ClientManager::handle_new_socket(sockets::Udp* socket){
	u_int32_t client_id = Client_ID;
	Client::s_ptr client = Client::s_ptr(new Client<sockets::Udp>(this, client_id, socket));
	clients[client_id] = client;
	Client_ID++;
}

void ClientManager::handle_new_socket(sockets::Tcp* socket){
	u_int32_t client_id = Client_ID;
	clients[client_id] = Client::s_ptr(new Client<sockets::Tcp>(this, client_id, socket));
	Client_ID++;
}

bool ClientManager::isAlive() {
	return alive.load();
}

bool ClientManager::on_enter(Client *client) {
	return manager->onEnter(client);
}

bool ClientManager::on_handshakerecv(Client *client, protocol::http::header* handshake, errors::error_code& e){
#ifdef USE_SESSIONS
	sessionManager->start_session(client, handshake, e);
#endif
	return manager->onHandshakeRecv(client, handshake);
}

bool ClientManager::on_handshakesend(Client *client, protocol::http::header* handshake, errors::error_code& e){
#ifdef USE_SESSIONS
	sessionManager->update_handshake(client, handshake, e);
#endif
	return manager->onHandshakeSend(client, handshake);
}

void ClientManager::on_ready(Client *client) {
	manager->onReady(client);
}

void ClientManager::on_receive(Client *client, protocol::packet::Packet *packet) {
#ifdef USE_MODULES
	for(protocol::packet::packet_iterator it = packet->begin() ; it != packet->end() ; ++it ){
		protocol::packet::PacketData *p = it->get();
		if (p->packetType == protocol::packet::MODULE){
			protocol::packet::ModulePacketData *packetData = reinterpret_cast<protocol::packet::ModulePacketData *>(p);
			modulesManager->onReceive(client, packetData);
		}
	}
#endif

	manager->onReceive(client, packet);

}

void ClientManager::on_close(Client *client) {
	DEBUG_PRINT("ClientManager::on_close");
	manager->onClose(client);

	client->alive.store(false);

#ifdef USE_SESSIONS
	sessionManager->close_session(client);
#endif

#ifdef USE_MODULES
	client->modulesController->unregisterAll();
#endif

	//@todo check lock block
	DEBUG_PRINT("worker_closingclients->dispatch(client->get_id())");
	worker_closingclients->dispatch(client->get_id());
}

void ClientManager::on_error(Client *client) {
	manager->onError(client);
}

#ifdef USE_MODULES
ModulesManager* ClientManager::getModulesManager(){
	return modulesManager;
}
#endif

void ClientManager::handleClientClosed(uint32_t clientId){
	if(clients.count(clientId) > 0) {
		DEBUG_PRINT("ClientManager::handleClientClosed : ",clientId);
		Client::s_ptr client = clients[clientId];
		assert(!client->isAlive());
		client->closeSocket();
		clients.erase(clientId);
		client.reset();
	}
}

void ClientManager::close(){
	alive.store(false);
	worker_closingclients->join(true);
}