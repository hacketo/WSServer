//
// Created by hacketo on 23/08/16.
//

#include "gtest/gtest.h"

int main(int argc, char* argv[]) {
	::testing::GTEST_FLAG(throw_on_failure) = true;
	::testing::InitGoogleTest(&argc,argv);
	RUN_ALL_TESTS();
}