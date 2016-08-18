//
// Created by hacketo on 11/08/16.
//


#include "gtest/gtest.h"

#define TEST_FRIENDS \
    friend class modules_test_null_initialized_check_Test;

#include "../../src/modules/base_module.h"



TEST(modules_test, null_initialized_check) {

	ModulesManager* manager = new ModulesManager();

	base_module* module = new base_module;

	manager->REGISTER(module);

	EXPECT_EQ(0, module->get_id());
	EXPECT_EQ(1, manager->MODULE_ID);
}

