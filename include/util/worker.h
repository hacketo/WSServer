//
// Created by hacketo on 13/08/16.
//

#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include <boost/thread/thread.hpp>
#include <atomic>
#include "safedeque.h"
#include "debug.h"


class Worker{

public:
	Worker();

	virtual ~Worker();

	virtual bool join(bool _interrupt = true, bool wait_end = false);

	virtual void interrupt(bool wait_end = false);

	virtual void init_job_thread();

protected:
	uint32_t _id;
	std::string debug_tag;
	std::atomic<bool> m_joined;
	std::atomic<bool> m_interrupted;
	boost::thread m_thread;
private:
	static uint32_t ID;
};



/**
 *
 */
template <class T>
class WorkerDeQue : public Worker {

public:
	WorkerDeQue(size_t size = 50) : Worker() , safeDeQue(size){}

	~WorkerDeQue(){
		if (!join(true) || !m_interrupted.load()){
			interrupt();
		}
	}

	virtual void init_job_thread(){
		m_thread = boost::thread(&WorkerDeQue<T>::job, this);
	}

	bool dispatch(T v){
		return safeDeQue.push(v);
	}

	void interrupt(bool wait_end = false) override{
		if (!m_interrupted.load()) {
			safeDeQue.interrupt(wait_end);
		}
	}

	virtual void do_job(T p) = 0;

protected:

	SafeDeQue<T> safeDeQue;

private:



	void job(){
		boost::thread::id tid = boost::this_thread::get_id();

		while (!m_interrupted.load()){
			DEBUG_PRINT(debug_tag,"-", _id, " \\");
			T s;
			if(safeDeQue.pop(s)) {
				do_job(s);
			}
			else{
				m_interrupted.store(true);
				DEBUG_PRINT(debug_tag, " Interrupted");
			}
			DEBUG_PRINT(debug_tag, " /");
		}
		DEBUG_PRINT(debug_tag," Ended");
	}
};

#endif //SERVER_WORKER_H
