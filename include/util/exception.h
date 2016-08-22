//
// Created by hacketo on 11/08/16.
//

#ifndef SERVER_EXCEPTION_H
#define SERVER_EXCEPTION_H

#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
	NotImplementedException() : std::logic_error("Function not yet implemented"){};
};

class CircularException : public std::logic_error {
public:
	CircularException() : std::logic_error("You can't add an object to itself"){};
};


class NotSupportedException : public std::logic_error {
public:
	NotSupportedException() : std::logic_error("Function not supported"){};
};

class ValueNotInitializedException : public std::logic_error {
public:
	ValueNotInitializedException(std::string type) : std::logic_error("Value of type not initialized"){};
};

#endif //SERVER_EXCEPTION_H
