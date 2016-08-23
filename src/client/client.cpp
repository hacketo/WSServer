#include <boost/algorithm/string.hpp>
#include <util/timer.h>
#include "client/client.h"

#include "debug.h"
#include "protocol/opcode.h"


//<editor-fold desc="Client">

Client::Client(ClientsManager* manager, u_int32_t client_id, boost::asio::io_service& io_service) :
		clientManager(manager), socket_(io_service){
    id =  client_id;
    alive = false;
    flag = 0;
	state_id = 0;
#ifdef USE_MODULES
	modulesController = ModulesController::create(this, clientManager->getModulesManager());
#endif
}

Client::~Client(){}

boost::asio::ip::tcp::socket& Client::socket(){
    return socket_;
}

void Client::start() {
	boost::asio::ip::tcp::endpoint ep = socket_.remote_endpoint();

	ip = ep.address().to_string();

	alive = clientManager->on_enter(this);
	errors::error_code e;

	if (alive){

		http::handshake* handshake = new http::handshake;

		get_http_header(handshake, e);
		if (!e) {

			// if handshake has cookie sid, récupérer la session
			alive = clientManager->on_handshakerecv(this, handshake, e);
			if (alive) {

				send_handshake(handshake->wsKey.c_str(), e);

				if (!e) {
					alive = clientManager->on_ready(this);
					if (alive) {
						incoming_worker = IncomingMessagesWorker::create(clientManager, this);
						outgoing_worker = OutgoingMessagesWorker::create(this);

						incoming_worker->init_job_thread();
						outgoing_worker->init_job_thread();
						incoming_worker->start_read_loop();
					}
				}
			}
		}

	}

	if (e) {
		DEBUG_PRINT(e);
		alive = false;
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

void Client::send(std::string& message) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
	outgoing_worker->dispatch(frame::from_string(message));
}
void Client::send(frame::FrameInterface *holder) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
	outgoing_worker->dispatch(holder->getFrame());
}
void Client::send(frame::Frame* frame) {
	//@todo check lock block
	DEBUG_PRINT("outgoing_worker->dispatch");
	outgoing_worker->dispatch(frame);
}


/**
 * Parse le premier header recu pour l'upgrade de la connexion
 */
void Client::get_http_header(http::handshake* handshake, errors::error_code& error){
    char data[2048];
    boost::system::error_code e;

    size_t bytes = recv_sync(data, e);
    if ( e ) {
		error = errors::get_error("Client", errors::WS_RECV_ERROR,
								 "Read headers failed: %s" , boost::system::system_error(e).what());
		return;
    }

    std::string header_str(data, bytes);

	handshake->headers_raw = header_str;
	http::parse_header(header_str, handshake, error);
	if(!error){
		http::validate_header(handshake, error);
	}
}

/**
 * Parse le premier header recu pour l'upgrade de la connexion
 */
void Client::send_handshake(const char* websocket_key, errors::error_code& error){

	http::handshake* handshake = http::get_handshake(websocket_key);

	alive = clientManager->on_handshakesend(this, handshake, error);

    boost::system::error_code e;

	uint8_t *data;
	uint64_t size;
	http::handshake_to_uint8(handshake, &data, size);
	send_sync(data, size, e);
    if ( e ) {
		error = errors::get_error("Client", errors::WS_SEND_ERROR, "Send handshake failed: %s", boost::system::system_error(e).what());
    }
}

size_t Client::recv_sync(char *data, boost::system::error_code &error){
    return socket_.read_some(boost::asio::buffer(data, 2048), error);
}
void Client::send_sync(unsigned char *data, size_t size,
					   boost::system::error_code &error){
    socket_.write_some(boost::asio::buffer(data, size), error);
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
		Worker<frame::FrameBuffer>(size), clientManager(manager), client(client){
}

void IncomingMessagesWorker::init_job_thread(){
	worker = boost::thread(&IncomingMessagesWorker::job, this);
}


void IncomingMessagesWorker::start_read_loop(){
	read_async_loop();
}
void IncomingMessagesWorker::read_async_loop(){
	client->socket_.async_read_some(boost::asio::buffer(temp_buffer, constant::max_buffer_size),
									boost::bind(&IncomingMessagesWorker::on_read_some, this,
												boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
	);
}
void IncomingMessagesWorker::on_read_some(const boost::system::error_code& error, std::size_t bytes_transferred){
	if (!error){
		DEBUG_PRINT("Received some stuff : ",bytes_transferred);
		dispatch(frame::from_uint8_t(temp_buffer, static_cast<uint16_t>(bytes_transferred), packetID));
		packetID++;
		if (client->alive){
			read_async_loop();
		}
	}
}

void IncomingMessagesWorker::job(){

	uint32_t frameSize = 0;
	uint32_t consumed = 0;
	bool hasSize = false;

	uint8_t* buffer;

	uint16_t lastFramebufferID = 0;

	while (client->alive){
		if(pool.is_not_empty()){
			frame::FrameBuffer frameBuffer;
			pool.pop_back(&frameBuffer);

			assert(lastFramebufferID == frameBuffer._id);
			lastFramebufferID++;

			uint32_t& bufferSize = frameBuffer.bufferSize;

			if(bufferSize == 0){
				continue;
			}

			// Si on a des données à exploité et que l'on a pas encore la taille de la frame
			// Cela veut dire que c'est le début d'une nouvelle frame
			if (bufferSize > 1 && !hasSize){
				hasSize = true;
				frameSize = std::max<uint32_t>(bufferSize, frame::get_framelen(&frameBuffer));
				buffer = new uint8_t[frameSize];
			}

			assert(buffer);

			memcpy(buffer+consumed,frameBuffer.buffer, bufferSize);

			consumed += bufferSize;

			if (consumed == frameSize){

				frame::Frame* frame = new frame::Frame;

				frame::decode(frame, buffer, frameSize);
				if (frame->op_code == opcode::CLOSE){
					client->alive = false;
					clientManager->on_close(client);
					continue;
				}

				errors::error_code e;
				packet::Packet::u_ptr p = packet::Packet::u_ptr(new packet::Packet());
				packet::parse(p.get(), frame->msg, e);

				delete[] buffer;

				clientManager->on_receive(client, p.get());

				consumed = 0;
				hasSize = false;
				frameSize = 0;
			}


		}
	}
	DEBUG_PRINT("IncomingMessagesWorker ",client->id, " ended");
}

//</editor-fold>

//<editor-fold desc="OutgoingMessagesWorker">

OutgoingMessagesWorker::OutgoingMessagesWorker(Client *client, size_t size) :
		Worker<frame::Frame*>(size), client(client){
}
void OutgoingMessagesWorker::init_job_thread() {
	worker = boost::thread(&OutgoingMessagesWorker::job, this);
}
void OutgoingMessagesWorker::job(){
	while (client->alive){
		if(pool.is_not_empty()){
			frame::Frame* frame;
			pool.pop_back(&frame);

			DEBUG_PRINT("outgoing_worker->job before encode");
			frame::encode(frame);
			DEBUG_PRINT("outgoing_worker->job before send");
			boost::system::error_code error;
			client->send_sync(frame->buffer,frame->bufferSize, error);

			if (error){
				DEBUG_PRINT("Send data failed: " , boost::system::system_error(error).what());
			}

			delete frame;
		}
	}
	DEBUG_PRINT("OutgoingMessagesWorker ",client->id, " ended");
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
	DEBUG_PRINT("Clients_Manager ClosingClientsWorker ended");
}

ClientsManager::ClientsManager(Manager* m) :
		Client_ID(0), alive(true), manager(Manager::u_ptr(m)) {

}

void ClientsManager::init(){
#ifdef USE_MODULES
	modulesManager = manager->getModulesManager();
#endif

	//worker_closingclients = ClosingClientsWorker::u_ptr(new ClosingClientsWorker(this));
	//worker_closingclients->init_job_thread();

#ifdef USE_SESSIONS
	sessionManager = SessionManager::u_ptr(new SessionManager);
	alive = sessionManager->alive;
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
	return manager->onEnter(client);
}

bool ClientsManager::on_handshakerecv(Client *client, http::handshake* handshake, errors::error_code& e){
#ifdef USE_SESSIONS
	sessionManager->start_session(client, handshake, e);
#endif
	return manager->onHandshakeRecv(client, handshake);
}

bool ClientsManager::on_handshakesend(Client *client, http::handshake* handshake, errors::error_code& e){
#ifdef USE_SESSIONS
	sessionManager->update_handshake(client, handshake, e);
#endif
	return manager->onHandshakeSend(client, handshake);
}

bool ClientsManager::on_ready(Client *client) {
	return manager->onReady(client);
}

void ClientsManager::on_receive(Client *client, packet::Packet *packet) {
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

void ClientsManager::on_close(Client *client) {
	DEBUG_PRINT("ClientsManager::on_close");
	manager->onClose(client);
#ifdef USE_SESSIONS
	sessionManager->close_session(client);
#endif

#ifdef USE_MODULES
	client->modulesController->unregisterAll();
#endif

	//@todo check lock block
	//worker_closingclients->dispatch(client->get_id());
}

void ClientsManager::on_error(Client *client) {
	manager->onError(client);
}

#ifdef USE_MODULES
ModulesManager* ClientsManager::getModulesManager(){
	return modulesManager;
}
#endif

void ClientsManager::handleClientClosed(uint32_t clientId){
	if(clients.count(clientId) > 0) {
		DEBUG_PRINT("ClientsManager::handleClientClosed : ",clientId);
		Client::s_ptr client = clients[clientId];
		assert(!client->isAlive());
		client->closeSocket();
		client->joinThreads();
		clients.erase(clientId);
		client.reset();
	}
}
