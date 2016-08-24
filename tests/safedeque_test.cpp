//
// Created by hacketo on 22/08/16.
//

#include <gtest/gtest.h>
#include "util/safedeque.h"

TEST(safedeque_test, testinit){

	SafeDeQue<std::string> deque(5);

	std::string a = "aa";
	std::string b = "bb";
	std::string c = "cc";
	std::string d = "dd";

	deque.push(a);

	EXPECT_EQ(1, deque.weight());

	deque.push(b);

	EXPECT_EQ(2, deque.weight());

	deque.push(c);

	EXPECT_EQ(3, deque.weight());

	deque.push(d);

	EXPECT_EQ(4, deque.weight());

	std::string t ;
	deque.pop(t);

	EXPECT_EQ(3, deque.weight());

	EXPECT_EQ(a, t);
}

TEST(safedeque_test, testpointer){

	SafeDeQue<std::string*> deque(5);

	std::string* a = new std::string("aa");
	std::string* b = new std::string("bb");

	deque.push(a);
	deque.push(b);

	EXPECT_EQ(2, deque.weight());

	std::string* t ;
	deque.pop(t);

	EXPECT_EQ(a, t);


	delete (a);
	delete (b);
}