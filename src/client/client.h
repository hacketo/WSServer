#ifndef CLIENT_H
#define CLIENT_H

#include "boost/asio.hpp"
#include "boost/thread/thread.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "../util/synchronizedpool.h"
#include "../protocol/http.h"
#include "../protocol/frame.h"
#include "../protocol/constant.h"
#include "../util/worker.h"
#include "../modules/base_module.h"
#include "manager/manager.h"
#include "../protocol/packet/packet.h"
#include "../server/config.h"


#if USE_SESSIONS
#include "../modules/sessionmanager.h"
#endif

using namespace protocol;

class ClientsManager;

/**
 * Thread safe worker pour recevoir et traiter les frames recue sur 2 threads différents
 * Décode les frames recues
 */
class IncomingMessagesWorker : public Worker<frame::Frame>{
public:
	typedef std::unique_ptr<IncomingMessagesWorker> u_ptr;
	static u_ptr create(ClientsManager* manager, Client *client, size_t size = 50){
		return u_ptr(new IncomingMessagesWorker(manager, client, size));
	}
	IncomingMessagesWorker(ClientsManager* manager, Client *client, size_t size = 50);
	void init_job_thread();
	void start_read_loop();
private:
	ClientsManager* clientManager;
	void read_async_loop();
	void on_read_some(const boost::system::error_code& error, std::size_t bytes_transferred);
	/**
	 * Buffer temporaire pour les nouveau message
	 */
	uint8_t temp_buffer[constant::max_buffer_size];

	Client* client;
	void job();
};


/**
 * Thread safe worker pour envoyer les messages au client
 * Recoit une liste de message à encoder et envoyer au client
 */
class OutgoingMessagesWorker : public Worker<frame::Frame> {
public:
	typedef std::unique_ptr<OutgoingMessagesWorker> u_ptr;
	static u_ptr create(Client *client, size_t size = 50){
		return u_ptr(new OutgoingMessagesWorker(client, size));
	}
	OutgoingMessagesWorker(Client *client, size_t size = 50);
	void init_job_thread();
private:
	Client* client;
	void job();
};


class Client : public boost::enable_shared_from_this<Client> {
	friend class IncomingMessagesWorker;
	friend class OutgoingMessagesWorker;
	friend class ClientsManager;
	friend class SessionManager;
public:

	virtual ~Client();

	typedef boost::shared_ptr<Client> s_ptr;

	static s_ptr create(ClientsManager* manager, u_int32_t client_id, boost::asio::io_service& io_service){
		return s_ptr(new Client(manager, client_id, io_service));
	}

	/**
	 * Référence de la socket tcp
	 * @return
	 */
	boost::asio::ip::tcp::socket& socket();

	/**
	 * Démarre la prise en compte du client,
	 * envoi le handshake ...
	 */
	void start();

	/**
	 * Envoi en message au client, thread safe
	 */
	void send(std::string message);

	/**
	 * Envoi en message au client, thread safe
	 */
	void send(frame::FrameInterface *holder);

	bool isAlive();

	bool registerModule(base_module* module);
	bool unregisterModule(base_module* module);
	bool hasModuleRegistered(uint64_t moduleId);
	ModuleClientController* getModuleController(uint64_t moduleId);



	std::string get_ip();
	uint32_t get_id();

	/**
	 * Utilisé pour savoir dans quel état le client est
	 * Utilisé avec le StateBasedManager
	 */
	u_int32_t state_id;

	/**
	 * Possibilité de stocker un flag pour ce client
	 */
	u_int32_t flag;


private:
	void joinThreads();
	void closeSocket();
	/**
	 * Ip du client
	 */
	std::string ip;

	/**
	 * Id du client pour le Clients_Manager
	 */
	uint32_t id;

	//@http://www.boost.org/doc/libs/1_58_0/doc/html/thread/thread_management.html#thread.thread_management.tutorial
	//@http://www.boost.org/doc/libs/1_58_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_server.cpp
	OutgoingMessagesWorker::u_ptr outgoing_worker;
	IncomingMessagesWorker::u_ptr incoming_worker;

	/**
	 * Détermine si on doit continuer la connexion avec ce client
	 */
    bool alive;

    boost::asio::ip::tcp::socket socket_;

	Client(ClientsManager* manager, u_int32_t client_id, boost::asio::io_service& io_service);

	/**
	 * Parse le premier header recu pour l'upgrade de la connexion
	 */
	void get_http_header(http::handshake* handshake, errors::error& error);

    /**
     * Envoi le handshake http au client
     * @param header
     */
	void send_handshake(const char* websocket_key, errors::error& error);

	size_t recv_sync(char *data, boost::system::error_code &error);

	void send_sync(unsigned char *data, size_t size, boost::system::error_code &error);

	ClientsManager* clientManager;
	ModulesController::u_ptr modulesController;
#if USE_SESSIONS
	Session* session;
#endif
};


class ClosingClientsWorker : public Worker<u_int32_t>{
public:
	typedef std::unique_ptr<ClosingClientsWorker> u_ptr;
	static u_ptr create(ClientsManager* manager, size_t size = 50){
		return u_ptr(new ClosingClientsWorker(manager, size));
	}
	ClosingClientsWorker(ClientsManager* manager,size_t size = 50);
	void init_job_thread();
private:
	ClientsManager* manager;
	void job();
};


class ClientsManager{

public:
	typedef std::unique_ptr<ClientsManager> u_ptr;

	ClientsManager(Manager* m);

	Client::s_ptr new_client(boost::asio::io_service &io_service);

	bool isAlive();

	/**
	 * Appelé lorsqu'un client se connecte, avant l'envoi du header
	 * donc pas possible dans cette méthode d'envoyer un message au client
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_enter(Client *client);

	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakerecv(Client *client, http::handshake* handshake, errors::error& e);
	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakesend(Client *client, http::handshake* handshake, errors::error& e);

	/**
	 * Appelé lorsqu'un client est prêt à communiquer avec le serveur
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_ready(Client *client);

	/**
	 * Appelé lorsque l'on recoit un message de la part d'un client
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 */
	void on_receive(Client *client, packet::Packet *packet);

	/**
	 * Appelé lorsqu'un client est déconnecté
	 * @param client
	 */
	void on_close(Client *client);

	/**
	 * Appelé lorqu'une érreur survient pour un client
	 * @param client
	 */
	void on_error(Client *client);

	void handleClientClosed(uint32_t client);

	ModulesManager* getModulesManager();
private:

#if USE_SESSIONS
	SessionManager::u_ptr sessionManager;
#endif

	ModulesManager* modulesManager;
	uint32_t Client_ID;
	Manager::u_ptr manager;
	bool alive;
	ClosingClientsWorker::u_ptr worker_closingclients;
	std::map<u_int32_t , Client::s_ptr> clients;
};



#endif // CLIENT_H
