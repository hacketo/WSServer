//
// Created by hacketo on 13/08/16.
//

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <boost/thread/thread.hpp>
#include "safedeque.h"

template <class T>
class Worker {

public:
	Worker(size_t size = 50) : pool(size){}
	~Worker(){
		join();
	}
	bool isAlive(){return alive;}
	void dispatch(T v){
		return pool.push_front(v);
	}

	void join(){
		alive = false;
		worker.join();
	}

protected:
	bool alive;
	boost::thread worker;
	SafeDeQue<T> pool;
};

#endif //SERVER_WORKER_H
