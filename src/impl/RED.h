//
// Created by hacketo on 12/08/16.
//

#ifndef SERVER_RED_H
#define SERVER_RED_H

#include "../client/manager/statebased_manager.h"

using namespace protocol::packet;


class ListFilesAction : public Action {
public:
	ListFilesAction(State *state);
	void handle(Client* client, GenericValue* packet);
};

class GetFileContentAction : public Action {
public:
	GetFileContentAction(State *state);
	void handle(Client* client, GenericValue* packet);
};


class MainState : public State {
public:
	static const u_int32_t ID = 1;
	MainState(StateBasedManager *manager);
	virtual void on_enter(Client* client) override;
	virtual void on_exit(Client* client) override;
	virtual void init_actions() override;
};


class LoginAction : public Action {
public:
	LoginAction(State *state);
	void handle(Client * client, GenericValue* packet);
};


class VoidState : public State {
public:
	static const u_int32_t ID = 0;
	VoidState(StateBasedManager *manager);
	virtual void init_actions() override;
};

#endif //SERVER_RED_H
