//
// Created by hacketo on 08/08/16.
//

#ifndef SERVER_DEBUG_H
#define SERVER_DEBUG_H

#include <iostream>
#include <fstream>
#include <sstream>

#ifndef WS_DEBUG
#define WS_DEBUG true
#endif

#if WS_DEBUG
#define DEBUG_PRINT(...) debug::print(__VA_ARGS__)
#else
#define DEBUG_PRINT(value)
#endif


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

    void log_data(std::string name, const unsigned char *data, size_t len);


	std::string getTabs(int depth);
}

#endif //SERVER_DEBUG_H
