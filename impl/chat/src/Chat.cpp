//
// Created by hacketo on 16/08/16.
//

#include "Chat.h"
#include "client/client.h"

using namespace protocol;

Chat::Chat() : Manager() {
	pubsub = new Pub_sub();
	modulesManager->REGISTER(pubsub);
}

bool Chat::onReady(Client *client) {
	client->registerModule(pubsub);

	std::stringstream msg;
	msg << "Client " << client->get_id() << " connecté !";
	packet::Packet::u_ptr p = packet::Packet::u_ptr(new packet::Packet(new StringValue(msg.str())));
	pubsub->publish(p.get());

	return true;
}

void Chat::onReceive(Client *client, packet::Packet *packet) {
	std::stringstream msg;
	msg << client->get_id() << " : " << packet->get(0)->data->getString();
	packet::Packet::u_ptr p = packet::Packet::u_ptr(new packet::Packet(new StringValue(msg.str())));
	pubsub->publish(p.get());
}

void Chat::onClose(Client *client) {
	client->unregisterModule(pubsub);
}