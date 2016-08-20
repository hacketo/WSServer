//
// Created by hacketo on 10/08/16.
//

#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H


#include "../../protocol/frame.h"
#include "../../protocol/packet/packet.h"
#include "../../modules/base_module.h"
#include "../../protocol/http.h"


class Client;

class Manager{

public:

	Manager(ModulesManager* modulesManager);

	typedef std::unique_ptr<Manager> u_ptr;

	/**
	 * Appelé lorsqu'un client se connecte, avant l'envoi du header
	 * donc pas possible dans cette méthode d'envoyer un message au client
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	virtual bool onEnter(Client *client);

	/**
	 * Appelé lorsqu'un le handshake à été recu et parsé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	virtual bool onHandshakeRecv(Client *client, protocol::http::handshake* handshake);

	/**
	 * Appelé le handshake est prêt à etre envoyé
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	virtual bool onHandshakeSend(Client *client, protocol::http::handshake* handshake);

	/**
	 * Appelé lorsqu'un client est prêt à communiquer avec le serveur
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 * @return
	 */
	virtual bool onReady(Client *client);

	/**
	 * Appelé lorsque l'on recoit un message de la part d'un client
	 * Si la méthode retourne False le client sera déconnecté
	 * @param client
	 */
	virtual void onReceive(Client *client, protocol::packet::Packet *packet);

	/**
	 * Appelé lorsqu'un client est déconnecté
	 * @param client
	 */
	virtual void onClose(Client *client);

	/**
	 * Appelé lorqu'une érreur survient pour un client
	 * @param client
	 */
	virtual void onError(Client *client);

	/**
	 * Appelé lors d'une érreur serveur
	 * @param client
	 */
	virtual void onServerError();

	/**
	 * Appelé lorsque le serveur se ferme
	 * @param client
	 */
	virtual void onExit();

	ModulesManager* getModulesManager();
protected:
	ModulesManager* modulesManager;
};


#endif //SERVER_MANAGER_H
