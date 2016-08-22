//
// Created by hacketo on 14/08/16.
//

#include "server/config.h"

#include "ext/modules/base_module.h"

#include "debug.h"
#include "client/client.h"

namespace ext {
namespace modules {
//<editor-fold desc="ModuleClientController">

	ModuleClientController::ModuleClientController(base_module *module) : module(module) {}

	void ModuleClientController::handle(Client *client, std::string& action, GenericValue *data) {}

//</editor-fold>

//<editor-fold desc="base_module">

	base_module::base_module() : alive(true){

	}

	uint64_t base_module::get_id() {
		return id;
	}

	ModuleClientController* base_module::getNewClientController(){}

	bool base_module::reg(Client *client, ModuleClientController **controller) {
		if (!is_client_already_subscribed(client)) {
			clients[client->get_id()] = client;
			*controller = getNewClientController();
			return true;
		}
		return false;
	}

	bool base_module::unregister(Client *client) {
		if (is_client_already_subscribed(client)) {
			clients.erase(client->get_id());
			return true;
		}
		return false;
	}


	bool base_module::is_client_already_subscribed(Client *client) {
		return algo::map_contains_key<uint32_t, Client *>(&clients, client->get_id());
	}

	bool base_module::isAlive() const{
		return alive;
	}
	base_module::client_iterator base_module::begin(){
		return clients.begin();
	}

	base_module::client_iterator base_module::end(){
		return clients.end();
	}

	errors::error base_module::join(){}

//</editor-fold>

//<editor-fold desc="ModulesManager">


	ModulesManager::ModulesManager() {
		MODULE_ID = 0;
	};

	bool ModulesManager::REGISTER(base_module *module) {
		if (is_module_registered(module)) {
			return false;
		}

		module->id = increment_moduleid();

		modules[module->id] = module;

		return true;
	}

	bool ModulesManager::is_module_registered(base_module *module) {
		uint64_t moduleId = module->get_id();
		/*
		if (moduleId == 0){
			return false;
		}*/

		if (algo::map_contains_key<uint64_t, base_module *>(&modules, moduleId)) {
			return modules[moduleId] == module;
		}
		return false;
	}

	bool ModulesManager::onReceive(Client *client, protocol::packet::ModulePacketData *packetData) {
		uint64_t moduleId = packetData->module;
		std::string action = packetData->action;

		if (client->hasModuleRegistered(moduleId)) {
			ModuleClientController *controller = client->getModuleController(moduleId);
			controller->handle(client, action, packetData->data.get());
			packetData->consumed = true;
			return true;
		}
		return false;
	}

	uint64_t ModulesManager::increment_moduleid() {
		return MODULE_ID++;
	}

//</editor-fold>

//<editor-fold desc="ModulesController">



	ModulesController::ModulesController(Client *client, ModulesManager *modulesManager) :
			client(client), modulesManager(modulesManager) {

	}

	bool ModulesController::reg(base_module *module) {
		if (modulesManager->is_module_registered(module)) {
			if (has(module->get_id())) {
				// Module déjà enregistré dans la liste client
				return false;
			}
			ModuleClientController *controller;
			if (module->reg(client, &controller)) {
				controllers[module->get_id()] = controller;
				return true;
			}
			if (controller) delete controller;
		}
		// Module non enregistré
		return false;
	}

	bool ModulesController::unregister(base_module *module) {
		if (has(module->get_id())) {
			// Module déjà enregistré dans la liste client
			module->unregister(client);
			controllers.erase(module->get_id());
			return true;
		}
		// Module non enregistré
		return false;
	}

	ModuleClientController *ModulesController::getModuleController(uint64_t moduleId) {
		if (algo::map_contains_key<uint64_t, ModuleClientController *>(&controllers, moduleId)) {
			return controllers[moduleId];
		}
		return nullptr;
	}

	bool ModulesController::has(uint64_t moduleId) {
		return algo::map_contains_key<uint64_t, ModuleClientController *>(&controllers, moduleId);
	}

	void ModulesController::unregisterAll() {
		for (module_iterator it = controllers.begin(); it != controllers.end(); ++it) {
			it->second->module->unregister(client);
		}
		controllers.clear();
	}
}
}
//</editor-fold>
