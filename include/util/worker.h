//
// Created by hacketo on 13/08/16.
//

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <boost/thread/thread.hpp>
#include "synchronizedpool.h"

template <class T>
class Worker {

public:
	Worker(size_t size = 50) : pool(size){}
	bool isAlive(){return alive;}
	void dispatch(T v){return pool.push_front(v);}
	void join(){worker.join();}

protected:
	bool alive;
	boost::thread worker;
	SynchronizedPool<T> pool;
};

#endif //SERVER_WORKER_H
