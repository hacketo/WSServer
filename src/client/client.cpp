#include <boost/algorithm/string.hpp>
#include <util/timer.h>
#include "client/client.h"
#include "client/clientmanager.h"

#include "debug.h"

#ifdef USE_MODULES
#include "ext/modules/base_module.h"
#endif


Client::Client(ClientManager* manager, u_int32_t client_id, sockets::Socket* socket) :
		clientManager(manager){

    id =  client_id;
	m_socket = sockets::Socket::u_ptr(socket);

	errors::error_code ec;
	m_socket->bind_client(this, ec);
	if (ec){
		alive.store(false);
	}
	else{
		alive.store(true);
	}

    flag = 0;
	state_id = 0;
#ifdef USE_MODULES
	modulesController = ModulesController::create(this, clientManager->getModulesManager());
#endif

}

std::string Client::get_ip() {
	return m_socket->get_ip();
}
uint32_t Client::get_id() {
	return id;
}


size_t Client::send(std::string& message, errors::error_code& ec) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
	return m_socket->send(message, ec);
}



#ifdef USE_MODULES
bool Client::registerModule(base_module* module){
	return modulesController->reg(module);
}
bool Client::unregisterModule(base_module* module){
	return modulesController->unregister(module);
}
bool Client::hasModuleRegistered(uint64_t moduleId){
	return modulesController->has(moduleId);
}

ModuleClientController* Client::getModuleController(uint64_t moduleId){
	return modulesController->getModuleController(moduleId);
}
#endif


bool Client::isAlive(){
	return alive.load();
}


void Client::closeSocket(){
	if (!isAlive()) {
		m_socket->close();
	}
}


bool Client::on_enter() {
	return clientManager->on_enter(this);
}


bool Client::on_handshakerecv(protocol::http::header* handshake, errors::error_code& e) {
	return clientManager->on_handshakerecv(this, handshake,e);
}


bool Client::on_handshakesend(protocol::http::header *handshake, errors::error_code &e) {
	return clientManager->on_handshakesend(this, handshake,e);
}


void Client::on_ready() {
	return clientManager->on_ready(this);
}


void Client::on_receive(protocol::packet::Packet *packet) {
	return clientManager->on_receive(this, packet);
}


void Client::on_close() {
	return clientManager->on_close(this);
}


void Client::on_error() {
	return clientManager->on_error(this);
}