//
// Created by hacketo on 08/08/16.
//

#ifndef SERVER_DEBUG_H
#define SERVER_DEBUG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "util/genericvalue.h"
#include "server/errors.h"

namespace debug {

	template <typename T>
	void print(T t){
		std::cout << t << std::endl ;
	}

	template<typename T, typename... Args>
	void print(T t, Args... args){
		std::cout << t;

		print(args...) ;
	}

	void print(errors::error &e);

    void log_data(std::string name, const unsigned char *data, size_t len);


	static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };


	void debugField(GenericValue *value , int baseDepth = 0);

	std::string getTypeOfField(rapidjson::Value::ConstMemberIterator &it);

	std::string getTabs(int depth);
}

#endif //SERVER_DEBUG_H
