//
// Created by hacketo on 19/08/16.
//

#include "worker.h"

template <class T>
Worker<T>::Worker(size_t size) : pool(size){

}
template <class T>
bool Worker<T>::isAlive(){
	return alive;
}

template <class T>
void Worker<T>::dispatch(T v){
	return pool.push_front(v);
}
template <class T>
void Worker<T>::join(){
	worker.join();
}
