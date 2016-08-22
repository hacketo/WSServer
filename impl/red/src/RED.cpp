//
// Created by hacketo on 12/08/16.
//

#include "RED.h"
#include "client/client.h"

ListFilesAction::ListFilesAction(State *state) : Action("dir", state){}
void ListFilesAction::handle(Client* client, GenericValue* packet){
	ObjectValue* objectValue = new ObjectValue();
	objectValue->set("dirs", new ArrayValue({new StringValue("files1"),new StringValue("files2")}));
	Packet::u_ptr p = Packet::u_ptr(new Packet("list_dir", objectValue));
	client->send(p.get());
}


GetFileContentAction::GetFileContentAction(State *state) : Action("file", state){}
void GetFileContentAction::handle(Client* client, GenericValue* packet){
	ObjectValue* objectValue = new ObjectValue();
	objectValue->set("content", new StringValue("file content"));
	Packet::u_ptr p = Packet::u_ptr(new Packet("file", objectValue));
	client->send(p.get());
}



MainState::MainState(StateBasedManager *manager) : State(MainState::ID, manager){
	//pubsub = new Pub_sub();
	//getModulesManager()->REGISTER(pubsub);

}
void MainState::on_enter(Client* client){
	//client->registerModule(pubsub);
}
void MainState::on_exit(Client* client){
	//client->unregisterModule(pubsub);
}

void MainState::init_actions(){
	add_actions(new ListFilesAction(this));
	add_actions(new GetFileContentAction(this));
}



LoginAction::LoginAction(State *state) : Action("login", state){}
void LoginAction::handle(Client * client, GenericValue* packet){
	std::string pseudo = (*packet)["pseudo"]->getString();
	std::string password = (*packet)["password"]->getString();
	if (pseudo == "hacketo" && password == "azerty") {
		updateClientState(client, MainState::ID);
		Packet::u_ptr p = Packet::u_ptr(new Packet("login_ok"));
		client->send(p.get());
	}
}


VoidState::VoidState(StateBasedManager *manager) : State(VoidState::ID, manager){}
void VoidState::init_actions(){
	add_actions(new LoginAction(this));
}

