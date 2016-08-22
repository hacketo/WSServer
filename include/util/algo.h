//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_ALGO_H
#define SERVER_ALGO_H

#include <algorithm>


#define VECTOR_CONTAINS(vec, value) (std::find(vec->begin(), vec->end(), value) != vec->end())

#define MAP_CONTAINS_KEY(map, key) (map.find(key) != map.end())


#define STRING_TO_UINT8(str, buffer, bufferSize) \
const char *string = str.c_str(); \
size_t offset = bufferSize > 0 ? std::min<size_t >(str.size(), bufferSize) : str.size(); \
std::copy(string, string + offset, buffer)


#define SPRINTF(str, token) \
uint64_t size = str.length() + token.length();\
char buffer[size]; \
std::sprintf(buffer, str.c_str(), token);\
str = std::string(buffer)

#endif //SERVER_ALGO_H
