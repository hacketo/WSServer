//
// Created by hacketo on 24/08/16.
//

#include "util/worker.h"

uint32_t Worker::ID = 1;

Worker::Worker() : interrupted(false), joined(false), _id(ID){
	ID++;
}

Worker::~Worker(){
	join();
}

bool Worker::join(bool _interrupt , bool wait_end){
	if(joined){
		return false;
	}
	if(_interrupt){
		interrupt(wait_end);
	}
	joined = true;
	m_thread.join();
	interrupt();
	return true;
}

void Worker::interrupt(bool wait_end){
	interrupted = true;
}

void Worker::init_job_thread(){};