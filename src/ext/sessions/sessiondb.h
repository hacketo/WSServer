//
// Created by hacketo on 20/08/16.
//

#ifndef SERVER_SESSIONDB_H
#define SERVER_SESSIONDB_H


#include "dbhandler.h"

class Session;

class SessionDB : public DbHandler {

public:
	SessionDB();
	errors::error open_database();

	errors::error saveSession(Session *pSession, bool ended);
	errors::error close();

private:

	uint64_t indexSessionSave;

	sqlite3_stmt* stmtSaveClient;
	sqlite3_stmt* stmtLastId;

};


#endif //SERVER_SESSIONDB_H
