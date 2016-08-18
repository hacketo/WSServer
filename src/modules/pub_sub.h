//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_PUB_SUB_H
#define SERVER_PUB_SUB_H


#include "../client/client.h"
#include "../util/algo.h"
#include "base_module.h"

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
	bool reg(Client* client) override;

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

	virtual ModuleClientController* getModuleClientController() override;


private:

	inline bool is_client_already_subscribed(Client* client){
		return algo::map_contains_key<uint32_t, Client*>(&clients, client->get_id());
	}

	std::map<uint32_t, Client*> clients;
	typedef std::map<uint32_t, Client*>::iterator client_iterator;
};


class Pub_subClientController : public ModuleClientController {
public:
	Pub_subClientController(base_module* module);
	virtual void handle(std::string action, GenericValue* data);
};

#endif //SERVER_PUB_SUB_H
