#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "sockets/socket.h"

#include "util/safedeque.h"

#include "protocol/http.h"

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
	size_t send(std::string& message, errors::error_code& ec);

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
	bool on_handshakerecv(protocol::http::header* handshake, errors::error_code& e);
	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakesend(protocol::http::header* handshake, errors::error_code& e);

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
	void on_receive(protocol::packet::Packet *packet);

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

#endif // CLIENT_H
