//
// Created by hacketo on 24/08/16.
//

#include "util/worker.h"

uint32_t Worker::ID = 1;

Worker::Worker() : _id(ID){
	m_interrupted.store(false);
	m_joined.store(false);
	ID++;
}

Worker::~Worker(){
	join();
}

bool Worker::join(bool _interrupt , bool wait_end){
	if(m_joined.load()){
		return false;
	}
	if(_interrupt){
		interrupt(wait_end);
	}
	m_joined.store(true);
	m_thread.join();
	interrupt();
	return true;
}

void Worker::interrupt(bool wait_end){
	m_interrupted.store(true);
}

void Worker::init_job_thread(){};