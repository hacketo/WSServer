//
// Created by hacketo on 10/08/16.
//

#ifndef SERVER_STATEBASEDMANAGER_H
#define SERVER_STATEBASEDMANAGER_H


#include "manager.h"
#include "protocol/packet.h"

class StateBasedManager;
class State;


class Action{

public:
	Action(std::string& name, State *state);

	std::string name;

	/**
	 * Gère l'action
	 * @param client
	 * @param packet
	 */
	virtual void handle(Client* client, GenericValue* packet);

protected:
	void updateClientState(Client *client, u_int32_t id, bool send = false);
	State *state;
};


class State{
public:
	State(u_int32_t id, StateBasedManager *manager);
	~State();

	/** Id de l'état, unique */
	u_int32_t id;

	/**
	 * Methode override pour ajouter les différentes actions possibles dans cet état
	 */
	virtual void init_actions();

	virtual void on_enter(Client* client);
	virtual void on_exit(Client* client);

	/**
	 * Ajoute une action possible pour cet état
	 */
	void add_actions(Action *action);

	/**
	 * Gère un packet et le dispatche à la bonne action
	 * @param client
	 * @param frame
	 */
	void handle(Client *client, protocol::packet::PacketData *packet);


#ifdef USE_MODULES
	ModulesManager* getModulesManager();
#endif
	StateBasedManager* getManager();

private:
	std::map<std::string , Action *> actions;
protected:
	StateBasedManager *manager;
};



class StateBasedManager : public Manager {

	class ChangeStatePacket : public protocol::packet::Packet{
	public:
		ChangeStatePacket(u_int32_t id) : protocol::packet::Packet("change_etat", new IntValue(id)){}
	};

public:

	std::function<protocol::packet::Packet*(u_int32_t)> getChangeStatePacket;

	StateBasedManager();
	~StateBasedManager();

	void initStates(std::vector<State *> states);

	void updateClientState(Client *client, u_int32_t id, bool send = false);

	virtual bool onEnter(Client *client) override;

	virtual void onReady(Client *client) override;

	/**
	 * Récupère une frame, transforme les donnée en packets
	 * Dispatche les packet à l'état du client
	 * @param client
	 * @param frame
	 */
	virtual void onReceive(Client *client, protocol::packet::Packet *packet) override;

	virtual void onClose(Client *client) override;

	virtual void onError(Client *client) override;

	virtual void onServerError() override;

	virtual void onExit() override;

	bool hasState(uint32_t state);

private:
	std::map<u_int32_t, State*> states;
};


#endif //SERVER_STATEBASEDMANAGER_H
