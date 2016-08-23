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

	deque.push_front(a);

	EXPECT_EQ(1, deque.weight());

	deque.push_front(b);

	EXPECT_EQ(2, deque.weight());

	deque.push_front(c);

	EXPECT_EQ(3, deque.weight());

	deque.push_front(d);

	EXPECT_EQ(4, deque.weight());

	std::string t ;
	deque.pop_back(t);

	EXPECT_EQ(3, deque.weight());

	EXPECT_EQ(a, t);
}

TEST(safedeque_test, testpointer){

	SafeDeQue<std::string*> deque(5);

	std::string* a = new std::string("aa");
	std::string* b = new std::string("bb");

	deque.push_front(a);
	deque.push_front(b);

	EXPECT_EQ(2, deque.weight());

	std::string* t ;
	deque.pop_back(t);

	EXPECT_EQ(a, t);


	delete (a);
	delete (b);
}