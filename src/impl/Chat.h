//
// Created by hacketo on 16/08/16.
//

#ifndef SERVER_CHAT_H
#define SERVER_CHAT_H


#include "../client/manager/manager.h"
#include "../ext/modules/pub_sub.h"

class Chat : public Manager {

public:
	Chat();

	virtual bool onReady(Client *client) override;

	virtual void onReceive(Client *client, protocol::packet::Packet *packet) override;

	virtual void onClose(Client *client) override;

	Pub_sub* pubsub;
};


#endif //SERVER_CHAT_H
