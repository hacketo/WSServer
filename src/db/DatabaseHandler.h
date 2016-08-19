//
// Created by hacketo on 19/08/16.
//

#ifndef SERVER_DATABASEHANDLER_H
#define SERVER_DATABASEHANDLER_H

#include <string>
#include "../server/errors.h"
#include <sqlite3.h> 

class DatabaseHandler {

private:
	std::string filename;

	errors::error open_database();
	errors::error create_table(char* sql);

	sqlite3 *database;

};


#endif //SERVER_DATABASEHANDLER_H
