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
	Worker(size_t size = 50) : safeDeQue(size), interrupted(false){}
	~Worker(){
		join();
	}
	void dispatch(T v){
		return safeDeQue.push_front(v);
	}

	void join(){
		interrupt();
		worker.join();
	}

	void interrupt(){
		interrupted = true;
		m_mutex.unlock();
	}

protected:
	bool interrupted;
	boost::mutex m_mutex;
	boost::thread worker;
	SafeDeQue<T> safeDeQue;


};

#endif //SERVER_WORKER_H
