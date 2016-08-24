//
// Created by hacketo on 23/08/16.
//


#include <util/worker.h>
#include <debug.h>
#include "gtest/gtest.h"


class TestWorker : public WorkerDeQue<std::string> {
public:
	TestWorker(std::vector<std::string>& list){
		_list = &list;
	}

	void do_job(std::string v){
		std::cout << v << std::endl;
		_list->push_back(v);
	}

	std::vector<std::string>* _list;
};


TEST(worker_test, test_init){

	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	std::vector<std::string> list;

	TestWorker* t = new TestWorker(list);

	t->init_job_thread();
//

	t->dispatch("Bonjour");
	t->dispatch("Bonjour");
	t->dispatch("Bonjour");

	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	t->dispatch("Bonjour");
	t->dispatch("Bonjour");
	t->dispatch("Bonjour");

	t->join(true, true);
	EXPECT_EQ(list.size(), 6);

}