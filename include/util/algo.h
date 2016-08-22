//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_ALGO_H
#define SERVER_ALGO_H

#include <algorithm>
#include <vector>
#include <map>

namespace algo {

	template <typename T, typename Z>
	bool vector_contains(std::vector<T>* vec, Z value){
		return std::find(vec->begin(), vec->end(), value) != vec->end();
	}
	template <typename T, typename A, typename Z>
	bool map_contains_key(std::map<T,A>* vec, Z key){
		return vec->find(key) != vec->end();
	}


};


#endif //SERVER_ALGO_H
