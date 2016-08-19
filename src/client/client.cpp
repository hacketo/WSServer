#include <boost/algorithm/string.hpp>
#include "client.h"

#include "../debug.h"
#include "../protocol/opcode.h"
#include "../server/errors.h"

using namespace protocol;

//<editor-fold desc="Client">

Client::Client(ClientsManager* manager, u_int32_t client_id, boost::asio::io_service& io_service) :
		clientManager(manager), socket_(io_service){
    id =  client_id;
    alive = false;
    flag = 0;
	state_id = 0;
	modulesController = new ModulesController(this, clientManager->getModulesManager());
}

Client::~Client(){}

boost::asio::ip::tcp::socket& Client::socket(){
    return socket_;
}

void Client::start() {
	boost::asio::ip::tcp::endpoint ep = socket_.remote_endpoint();
	ip = ep.address().to_string();

	alive = clientManager->on_enter(this);
	if (alive){

		protocol::http::http_header header;

		if (get_http_header(header)) {


			if (send_handshake(header)) {
				alive = clientManager->on_ready(this, header);
				if (alive) {
					incoming_worker = IncomingMessagesWorker::create(clientManager, this);
					outgoing_worker = OutgoingMessagesWorker::create(this);

					incoming_worker->init_job_thread();
					outgoing_worker->init_job_thread();
					incoming_worker->start_read_loop();
				}
			}
			else {
				debug::print("Handshake problem /!");
			}
		}
		else {
			debug::print("Header problem !");
		}
	}

    if (!alive){
        socket_.close();
    }
}

std::string Client::get_ip() {
	return ip;
}
uint32_t Client::get_id() {
	return id;
}

void Client::send(std::string message) {
	//@todo check lock block
	outgoing_worker->dispatch(protocol::frame::from_string(message));
}
void Client::send(protocol::frame::FrameInterface *holder) {
	//@todo check lock block
	outgoing_worker->dispatch(holder->getFrame());
}


/**
 * Parse le premier header recu pour l'upgrade de la connexion
 */
bool Client::get_http_header(protocol::http::http_header& header){
    char data[2048];
    boost::system::error_code error;

    size_t bytes = recv_sync(data, error);
    if ( error ) {
        std::cerr << "Read header failed: " << boost::system::system_error(error).what() << std::endl;
        return false;
    }

    std::string header_str(data, bytes);
	protocol::http::parse_header(header_str, header);
#if USE_SESSIONS
	session->setHeader(header_str);
#endif
    return protocol::http::validate_header(header);
}

/**
 * Parse le premier header recu pour l'upgrade de la connexion
 */
bool Client::send_handshake(protocol::http::http_header& header){

    protocol::http::handshake handshake =
			protocol::http::get_handshake("ServerName", header["Sec-WebSocket-Key"], "http://red.hacketo.lan/", "ws://127.0.0.1:9876");

    boost::system::error_code error;

	uint64_t size = handshake.header.size();
    uint8_t data[size];
	string::convert(handshake.header, data);
	send_sync(data, size, error);
    if ( error ) {
        std::cerr << "Send handshake failed: " <<
				boost::system::system_error(error).what() << std::endl;
        return false;
    }
    return true;
}

size_t Client::recv_sync(char *data, boost::system::error_code &error){
    return socket_.read_some(boost::asio::buffer(data, 2048), error);
}
void Client::send_sync(unsigned char *data, size_t size,
					   boost::system::error_code &error){
    socket_.write_some(boost::asio::buffer(data, size), error);
}


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

bool Client::isAlive(){
	return alive;
}

void Client::closeSocket(){
	if (!alive) {
		socket_.close();
	}
}
void Client::joinThreads(){
	if (!alive) {
		outgoing_worker->join();
		incoming_worker->join();
	}
}

//</editor-fold>

//<editor-fold desc="IncomingMessagesWorker">

IncomingMessagesWorker::IncomingMessagesWorker(ClientsManager* manager, Client *client, size_t size) :
		Worker<protocol::frame::Frame>(size), clientManager(manager), client(client){
}

void IncomingMessagesWorker::init_job_thread(){
	worker = boost::thread(&IncomingMessagesWorker::job, this);
}


void IncomingMessagesWorker::start_read_loop(){
	read_async_loop();
}
void IncomingMessagesWorker::read_async_loop(){
	client->socket_.async_read_some(boost::asio::buffer(temp_buffer, protocol::constant::max_buffer_size),
									boost::bind(&IncomingMessagesWorker::on_read_some, this,
												boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
	);
}
void IncomingMessagesWorker::on_read_some(const boost::system::error_code& error, std::size_t bytes_transferred){
	if (!error){
		dispatch(protocol::frame::from_uint8_t(temp_buffer, static_cast<uint16_t>(bytes_transferred)));
		if (client->alive){
			read_async_loop();
		}
	}
}

void IncomingMessagesWorker::job(){
	while (client->alive){
		if(pool.is_not_empty()){
			protocol::frame::Frame frame;
			pool.pop_back(&frame);

			protocol::frame::decode(&frame);
			if (frame.op_code == protocol::opcode::CLOSE){
				client->alive = false;
				clientManager->on_close(client);
				continue;
			}

			errors::error e;
			protocol::packet::Packet::u_ptr p = protocol::packet::Packet::u_ptr(new protocol::packet::Packet());
			protocol::packet::parse(p.get(), frame.msg, e);

			clientManager->on_receive(client, p.get());
		}
	}
	debug::print("IncomingMessagesWorker ",client->id, " ended");
}

//</editor-fold>

//<editor-fold desc="OutgoingMessagesWorker">

OutgoingMessagesWorker::OutgoingMessagesWorker(Client *client, size_t size) :
		Worker<protocol::frame::Frame>(size), client(client){
}
void OutgoingMessagesWorker::init_job_thread() {
	worker = boost::thread(&OutgoingMessagesWorker::job, this);
}
void OutgoingMessagesWorker::job(){
	while (client->alive){
		if(pool.is_not_empty()){
			protocol::frame::Frame frame;
			pool.pop_back(&frame);

			protocol::frame::encode(&frame);

			boost::system::error_code error;
			client->send_sync(frame.buffer,frame.bufferSize, error);

			if (error){
				debug::print("Send data failed: " , boost::system::system_error(error).what());
			}
		}
	}
	debug::print("OutgoingMessagesWorker ",client->id, " ended");
}

//</editor-fold>




ClosingClientsWorker::ClosingClientsWorker(ClientsManager* manager, size_t size) :
		Worker<u_int32_t>(size), manager(manager){
}
void ClosingClientsWorker::init_job_thread(){
	worker = boost::thread(&ClosingClientsWorker::job, this);
}


void ClosingClientsWorker::job(){
	while (manager->isAlive()){
		if(pool.is_not_empty()){

			u_int32_t clientId;
			pool.pop_back(&clientId);
			manager->handleClientClosed(clientId);
		}
	}
	debug::print("Clients_Manager ClosingClientsWorker ended");
}

ClientsManager::ClientsManager(Manager* m) :
		Client_ID(0), alive(true), manager(Manager::unique_ptr(m)), modulesManager(m->getModulesManager()){

	worker_closingclients = ClosingClientsWorker::create(this);
	worker_closingclients->init_job_thread();
#if USE_SESSIONS
	sessionManager = new SessionManager;
#endif
}

Client::s_ptr ClientsManager::new_client(boost::asio::io_service &io_service){
	u_int32_t client_id = Client_ID;
	Client::s_ptr client = Client::create(this, client_id, io_service);
	clients[client_id] = client;
	Client_ID++;
	return client;
}

bool ClientsManager::isAlive() {
	return alive;
}

bool ClientsManager::on_enter(Client *client) {
#if USE_SESSIONS
	sessionManager->start_session(client);
#endif
	return manager->onEnter(client);
}

bool ClientsManager::on_ready(Client *client, protocol::http::http_header& map) {
	return manager->onReady(client, map);
}

void ClientsManager::on_receive(Client *client, packet::Packet *packet) {

	for(packet::packet_iterator it = packet->begin() ; it != packet->end() ; ++it ){
		packet::PacketData *p = it->get();
		if (p->packetType == packet::MODULE){
			packet::ModulePacketData *packetData = reinterpret_cast<packet::ModulePacketData *>(p);
			modulesManager->onReceive(client, packetData);
		}
	}

	manager->onReceive(client, packet);

}

void ClientsManager::on_close(Client *client) {
	manager->onClose(client);
#if USE_SESSIONS
	sessionManager->close_session(client);
#endif

	client->modulesController->unregisterAll();
	//@todo check lock block
	worker_closingclients->dispatch(client->get_id());
}

void ClientsManager::on_error(Client *client) {
	manager->onError(client);
}

ModulesManager* ClientsManager::getModulesManager(){
	return modulesManager;
}

void ClientsManager::handleClientClosed(uint32_t clientId){
	if(clients.count(clientId) > 0) {
		Client::s_ptr client = clients[clientId];
		assert(!client->isAlive());
		client->closeSocket();
		client->joinThreads();
		clients.erase(clientId);
		client.reset();
	}
}
