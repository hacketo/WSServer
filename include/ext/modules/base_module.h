//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_MODULE_H
#define SERVER_MODULE_H

#ifndef TEST_FRIENDS
#define TEST_FRIENDS
#endif

#include <cstdint>
#include <map>
#include "util/algo.h"
#include "protocol/packet.h"

class Client;

namespace ext {
namespace modules {
	class base_module;

//<editor-fold desc="ModuleClientController">

	class ModuleClientController {
	public:

		ModuleClientController(base_module *module);

		/**
		 *
		 * @param action
		 * @param data
		 * @return
		 */
		virtual void handle(Client *client, std::string& action, GenericValue *data);

		base_module *module;
	};

//</editor-fold>

//<editor-fold desc="base_module">

	class base_module {
		friend class ModulesManager;

	public:
		typedef std::map<uint32_t, Client *>::iterator client_iterator;

		base_module();

		uint64_t get_id();

		/**
		 * Appelé pour qu'un client subsribe a ce module
		 * @THREAD2
		 * @param client
		 * @return
		 */
		bool reg(Client *client, ModuleClientController **controller);

		/**
		 * Appelé pour qu'un client unsubsribe a ce Pub_sub
		 * @THREAD2
		 * @param client
		 * @return
		 */
		bool unregister(Client *client);
		bool unregister(uint32_t clientId);

		bool isAlive() const;


		client_iterator begin();
		client_iterator end();

		uint32_t nb_clients_registered();
	protected:
		bool alive;

		virtual ModuleClientController* getNewClientController();

		std::map<uint32_t, Client *> clients;

		virtual errors::error_code join();

	private:
		uint64_t id = 0;
	};

//</editor-fold>

//<editor-fold desc="ModulesManager">

	class ModulesManager {

		TEST_FRIENDS;
	public:
		ModulesManager();

		bool REGISTER(base_module *module);

		bool is_module_registered(base_module *module);

		bool onReceive(Client *client, protocol::packet::ModulePacketData *packetData);

	private:
		uint64_t increment_moduleid();

		uint64_t MODULE_ID;
		std::map<uint64_t, base_module *> modules;

	};


//</editor-fold>

//<editor-fold desc="ModulesController">

	class ModulesController {

	public:
		typedef std::unique_ptr<ModulesController> u_ptr;
		typedef std::map<uint64_t, ModuleClientController *>::iterator module_iterator;

		static u_ptr create(Client *client, ModulesManager *moduleManager) {
			return u_ptr(new ModulesController(client, moduleManager));
		}

		/**
		 * Enregistre le module passé en paramètre si il est valide,
		 * Récupère un modèle client pour ce module
		 * @param module
		 * @return
		 */
		bool reg(base_module *module);

		/**
		 * Supprime le modele client du module
		 * @param module
		 * @return
		 */
		bool unregister(base_module *module);


		bool has(uint64_t moduleId);

		ModuleClientController *getModuleController(uint64_t moduleId);

		void unregisterAll();

	private:
		ModulesController(Client *client, ModulesManager *moduleManager);

		ModulesManager *modulesManager;
		Client *client;
		std::map<uint64_t, ModuleClientController *> controllers;

	};
}
}
//</editor-fold>

#endif //SERVER_MODULE_H
