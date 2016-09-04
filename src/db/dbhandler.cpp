//
// Created by hacketo on 19/08/16.
//

#include "db/dbhandler.h"
#include "debug.h"
#include <boost/filesystem.hpp>

const char * DbHandler::DP_PATH = "";

DbHandler::DbHandler() : error(false){

}
DbHandler::~DbHandler(){
	this->close();
}

sqlite3_stmt* DbHandler::make_statement( const char * sql ){
	sqlite3_stmt* statement;
	sqlite3_prepare_v2( database, sql, -1, &statement, NULL );
	return statement;
}

bool DbHandler::is_error() {
	return error;
}

error::code DbHandler::open_database(const char* filename) {

	int  rc;
	error::code e;
	/* Open database */
	this->filename = filename;

	boost::filesystem::path path(DP_PATH);

	path += filename;

	if(!boost::filesystem::exists(path)) {
		error::get_code(e, error::DB_NEED_CREATE_TABLE);
	}

	rc = sqlite3_open(path.c_str(), &database);
	if (rc) {
		error::get_code(e, "DbHandler", error::DB_CANT_OPEN, "Can't open database: %s\n", get_dberror());
	}

	return e;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}



error::code DbHandler::exec(const char* sql){
	char *zErrMsg = 0;
	error::code e;


	/* Create SQL statement
	sql = "CREATE TABLE COMPANY("  \
         "ID INT PRIMARY KEY     NOT NULL," \
         "NAME           TEXT    NOT NULL," \
         "AGE            INT     NOT NULL," \
         "ADDRESS        CHAR(50)," \
         "SALARY         REAL );";*/

	/* Execute SQL statement */
	int  rc = sqlite3_exec(database, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		error::get_code(e, "DbHandler", error::SQL_ERROR, "SQL value: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	return e;
}

error::code DbHandler::close(){
	error::code e;

	sqlite3_close(database);


	return e;
}

const char *DbHandler::get_dberror() {
	return sqlite3_errmsg(database);
}
