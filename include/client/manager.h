//
// Created by hacketo on 10/08/16.
//

#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include "server/config.h"
#include "protocol/frame.h"
#include "protocol/packet.h"
#include "protocol/http.h"

#ifdef USE_MODULES
#include "ext/modules/base_module.h"
using ext::modules::ModulesManager;
#endif

class Client;

class Manager{

public:

	Manager();

	typedef std::unique_ptr<Manager> u_ptr;

	/**
	 * Appelé lorsqu'un client se connecte, avant l'envoi du header
	 * donc pas possible dans cette méthode d'envoyer un message au client
	 * Si la méthode retourne False le client sera déconnecté
	 * @return
	 */
	virtual bool onEnter(Client *client);

	/**
	 * Appelé lorsqu'un le handshakeClient à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @return
	 */
	virtual bool onHandshakeRecv(Client *client, protocol::http::handshake* handshake);

	/**
	 * Appelé lorsque le handshakeRéponse est prêt à etre envoyé
	 * Si la méthode retourne False le client sera déconnecté
	 * @return
	 */
	virtual bool onHandshakeSend(Client *client, protocol::http::handshake* handshake);

	/**
	 * Appelé lorsqu'un client est prêt à communiquer avec le serveur
	 * Si la méthode retourne False le client sera déconnecté
	 * @return
	 */
	virtual void onReady(Client *client);

	/**
	 * Appelé lorsque l'on recoit un message de la part d'un client
	 * Si la méthode retourne False le client sera déconnecté
	 */
	virtual void onReceive(Client *client, protocol::packet::Packet *packet);

	/**
	 * Appelé lorsqu'un client est déconnecté
	 */
	virtual void onClose(Client *client);

	/**
	 * Appelé lorqu'une érreur survient pour un client
	 */
	virtual void onError(Client *client);

	/**
	 * Appelé lors d'une érreur serveur
	 */
	virtual void onServerError();

	/**
	 * Appelé lorsque le serveur se ferme
	 */
	virtual void onExit();

#ifdef USE_MODULES
	ModulesManager* getModulesManager();
protected:
	ModulesManager* modulesManager;
#endif

};


#endif //SERVER_MANAGER_H
