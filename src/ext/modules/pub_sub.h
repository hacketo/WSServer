//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_PUB_SUB_H
#define SERVER_PUB_SUB_H


#include <stdint.h>
#include "base_module.h"
#include <memory>

class Client;

class Pub_sub : public base_module{

	friend class Pub_subModule;

public:

	typedef std::unique_ptr<Pub_sub> u_ptr;
	/**
	 * Appelé pour qu'un client subsribe a ce Pub_sub
	 * @THREAD2
	 * @param client
	 * @return
	 */
	bool reg(Client* client, ModuleClientController** controller) override;

	/**
	 * Appelé pour qu'un client unsubsribe a ce Pub_sub
	 * @param client
	 * @return
	 */
	bool unregister(Client* client) override;

	/**
	 * Appelé pour publier un message auc clients abonnés
	 * @THREAD1
	 * @param packet
	 * @return
	 */
	bool publish(protocol::frame::FrameInterface* frameInterface);

private:

	bool is_client_already_subscribed(Client* client);

	std::map<uint32_t, Client*> clients;
	typedef std::map<uint32_t, Client*>::iterator client_iterator;
};


class Pub_subClientController : public ModuleClientController {
public:
	Pub_subClientController(base_module* module);
	virtual void handle(Client* client, std::string action, GenericValue* data);
};

#endif //SERVER_PUB_SUB_H
