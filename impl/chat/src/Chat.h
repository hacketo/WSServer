//
// Created by hacketo on 16/08/16.
//

#ifndef SERVER_CHAT_H
#define SERVER_CHAT_H


#include "client/manager.h"
#include "ext/modules/pub_sub.h"

using ext::modules::pubsub::Pub_sub;

class Chat : public Manager {

public:
	Chat();

	/**
	 * Appelé lorsqu'un client est prêt à communiquer avec le serveur
	 * Si la méthode retourne False le client sera déconnecté
	 * @return
	 */
	virtual void onReady(Client *client) override;


	/**
	 * Appelé lorsque l'on recoit un message de la part d'un client
	 * Si la méthode retourne False le client sera déconnecté
	 */
	virtual void onReceive(Client *client, protocol::packet::Packet *packet) override;

	/**
	 * Appelé lorsqu'un client est déconnecté
	 */
	virtual void onClose(Client *client) override;

	Pub_sub* pubsub;
};


#endif //SERVER_CHAT_H
