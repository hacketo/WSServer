#include <boost/algorithm/string.hpp>
#include <util/timer.h>
#include "client/client.h"
#include "sockets/socket.h"

#include "debug.h"
#include "protocol/opcode.h"

#ifdef USE_MODULES
#include "ext/modules/base_module.h"
#endif

//<editor-fold desc="Client">

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


void Client::send(std::string& message) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
	frame::Frame* frame = frame::from_string(message);
//	if (!outgoing_worker->dispatch(frame)){
//		delete frame;
//	}
}

/**
 * Utilisé pour envoyer une frame à 1 seule personne
 * @param frame
 */
void Client::send(frame::FrameInterface *holder) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
	frame::Frame* frame = holder->getFrame();
//	if (!outgoing_worker->dispatch(frame)){
//		delete frame;
//	}
}

/**
 * Utilisé pour envoyer une frame à x personnes
 * @param frame
 */
void Client::send(frame::Frame* frame) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
//	if (!outgoing_worker->dispatch(frame)){
//		frame->m_count --;
//	}
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

bool Client::on_handshakerecv(http::handshake* handshake, errors::error_code& e) {
	return clientManager->on_handshakerecv(this, handshake,e);
}

bool Client::on_handshakesend(http::handshake *handshake, errors::error_code &e) {
	return clientManager->on_handshakesend(this, handshake,e);
}

void Client::on_ready() {
	return clientManager->on_ready(this);
}

void Client::on_receive(packet::Packet *packet) {
	return clientManager->on_receive(this, packet);
}

void Client::on_close() {
	return clientManager->on_close(this);
}

void Client::on_error() {
	return clientManager->on_error(this);
}
//</editor-fold>


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

void ClientManager::handle_new_socket(sockets::UdpSocket* socket){
	u_int32_t client_id = Client_ID;
	Client::s_ptr client = Client::s_ptr(new Client(this, client_id, socket));
	clients[client_id] = client;
	Client_ID++;
}

void ClientManager::handle_new_socket(sockets::TcpSocket* socket){
	u_int32_t client_id = Client_ID;
	clients[client_id] = Client::s_ptr(new Client(this, client_id, socket));
	Client_ID++;
}

bool ClientManager::isAlive() {
	return alive.load();
}

bool ClientManager::on_enter(Client *client) {
	return manager->onEnter(client);
}

bool ClientManager::on_handshakerecv(Client *client, http::handshake* handshake, errors::error_code& e){
#ifdef USE_SESSIONS
	sessionManager->start_session(client, handshake, e);
#endif
	return manager->onHandshakeRecv(client, handshake);
}

bool ClientManager::on_handshakesend(Client *client, http::handshake* handshake, errors::error_code& e){
#ifdef USE_SESSIONS
	sessionManager->update_handshake(client, handshake, e);
#endif
	return manager->onHandshakeSend(client, handshake);
}

void ClientManager::on_ready(Client *client) {
	manager->onReady(client);
}

void ClientManager::on_receive(Client *client, packet::Packet *packet) {
#ifdef USE_MODULES
	for(packet::packet_iterator it = packet->begin() ; it != packet->end() ; ++it ){
		packet::PacketData *p = it->get();
		if (p->packetType == packet::MODULE){
			packet::ModulePacketData *packetData = reinterpret_cast<packet::ModulePacketData *>(p);
			modulesManager->onReceive(client, packetData);
		}
	}
#endif

	manager->onReceive(client, packet);

}

void ClientManager::on_close(Client *client) {
	DEBUG_PRINT("ClientManager::on_close");
	manager->onClose(client);
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
