//
// Created by hacketo on 19/08/16.
//

#ifndef SERVER_DATABASEHANDLER_H
#define SERVER_DATABASEHANDLER_H

#include <string>
#include "server/errors.h"
#include <sqlite3.h> 

class DbHandler {

public:
	DbHandler();
	~DbHandler();

	errors::error_code open_database(const char* filename);
	errors::error_code exec(const char* sql);
	const char * get_dberror();

	virtual errors::error_code close();

	sqlite3_stmt* make_statement( const char * sql );

	bool is_error();
protected:
	bool error;

private:
	sqlite3 *database;
	static const char * DP_PATH;
	std::string filename;

};


#endif //SERVER_DATABASEHANDLER_H
