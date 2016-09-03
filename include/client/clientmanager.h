//
// Created by hacketo on 03/09/16.
//

#ifndef WSSERVERLIB_CLIENTMANAGER_H
#define WSSERVERLIB_CLIENTMANAGER_H

#include "client.h"
#include "sockets/tcp.h"
#include "sockets/udp.h"

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

	void handle_new_socket(sockets::Udp* socket);
	void handle_new_socket(sockets::Tcp* socket);

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
	bool on_handshakerecv(Client *client, http::header* handshake, errors::error_code& e);
	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	bool on_handshakesend(Client *client, http::header* handshake, errors::error_code& e);

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



#endif //WSSERVERLIB_CLIENTMANAGER_H
