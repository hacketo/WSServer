//
// Created by hacketo on 11/08/16.
//


#include "gtest/gtest.h"

#define private public

#include "ext/modules/base_module.h"



TEST(modules_test, null_initialized_check) {

	ext::modules::ModulesManager* manager = new ext::modules::ModulesManager();

	ext::modules::base_module* module = new ext::modules::base_module;

	manager->REGISTER(module);

	EXPECT_EQ(0, module->get_id());
	EXPECT_EQ(1, manager->MODULE_ID);
}

