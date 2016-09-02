#ifndef CLIENT_H
#define CLIENT_H

#include <sockets/socket.h>
#include "boost/asio.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "util/safedeque.h"
#include "protocol/http.h"
#include "protocol/frame.h"
#include "protocol/constant.h"
#include "util/worker.h"

#include "manager.h"
#include "protocol/packet.h"

#ifdef USE_MODULES
#include "ext/modules/base_module.h"
using namespace ext::modules;
#endif

#ifdef USE_SESSIONS
#include "ext/sessions/sessionmanager.h"
using namespace ext::sessions;
#endif



//#ifndef MOCK_SOCKET
//#define SOCKET_CLASS boost::asio::ip::tcp::sockets
//#else
//#include "mocks/socket_mock.h"
//#define SOCKET_CLASS MockSocket
//#endif

using namespace protocol;


class ClientManager;


class Client : public boost::enable_shared_from_this<Client> {
	friend class ClientManager;
#ifdef USE_SESSIONS
	friend class ext::sessions::SessionManager;
#endif
public:

	typedef boost::shared_ptr<Client> s_ptr;

	/**
	 * Envoi en message au client, thread safe
	 */
	void send(std::string& message);

	/**
	 * Envoi en message au client, thread safe
	 */
	void send(frame::FrameInterface *holder);

	void send(frame::Frame* frame);

	bool isAlive();



	/**
	 * Appelé lorsqu'un client se connecte, avant l'envoi du header
	 * donc pas possible dans cette méthode d'envoyer un message au client
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_enter();

	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakerecv(http::handshake* handshake, errors::error_code& e);
	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakesend(http::handshake* handshake, errors::error_code& e);

	/**
	 * Appelé lorsqu'un client est prêt à communiquer avec le serveur
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	void on_ready();

	/**
	 * Appelé lorsque l'on recoit un message de la part d'un client
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 */
	void on_receive(packet::Packet *packet);

	/**
	 * Appelé lorsqu'un client est déconnecté
	 * @param client
	 */
	void on_close();

	/**
	 * Appelé lorqu'une érreur survient pour un client
	 * @param client
	 */
	void on_error();


#ifdef USE_MODULES
	bool registerModule(base_module* module);

	bool unregisterModule(base_module* module);

	bool hasModuleRegistered(uint64_t moduleId);

	ModuleClientController* getModuleController(uint64_t moduleId);
#endif


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


protected:
	void closeSocket();

	/**
	 * Id du client pour le Clients_Manager
	 */
	uint32_t id;

	/**
	 * Détermine si on doit continuer la connexion avec ce client
	 */
	std::atomic<bool> alive;

	Client(ClientManager* manager, u_int32_t client_id, sockets::Socket* socket);

	ClientManager* clientManager;

	sockets::Socket::u_ptr m_socket;

#ifdef USE_MODULES
	ModulesController::u_ptr modulesController;
#endif

#ifdef USE_SESSIONS
	Session* session;
#endif
};


class ClosingClientsWorker : public WorkerDeQue<u_int32_t>{
public:
	typedef std::unique_ptr<ClosingClientsWorker> u_ptr;
	static u_ptr create(ClientManager* manager, size_t size = 50){
		return u_ptr(new ClosingClientsWorker(manager, size));
	}
	ClosingClientsWorker(ClientManager* manager,size_t size = 50);

	void do_job(u_int32_t id) override;
private:
	ClientManager* manager;
};


class ClientManager{

public:
	typedef std::unique_ptr<ClientManager> u_ptr;

	ClientManager(Manager* m);

	void handle_new_socket(sockets::UdpSocket* socket);
	void handle_new_socket(sockets::TcpSocket* socket);

	void init();

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
	bool on_handshakerecv(Client *client, http::handshake* handshake, errors::error_code& e);
	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakesend(Client *client, http::handshake* handshake, errors::error_code& e);

	/**
	 * Appelé lorsqu'un client est prêt à communiquer avec le serveur
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	void on_ready(Client *client);

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

	void close();

#ifdef USE_MODULES
	ModulesManager* getModulesManager();
#endif

private:

#ifdef USE_SESSIONS
	SessionManager::u_ptr sessionManager;
#endif
#ifdef USE_MODULES
	ModulesManager* modulesManager;
#endif

	uint32_t Client_ID;
	Manager::u_ptr manager;
	std::atomic<bool> alive;
	ClosingClientsWorker::u_ptr worker_closingclients;
	std::map<u_int32_t , Client::s_ptr> clients;
};



#endif // CLIENT_H
