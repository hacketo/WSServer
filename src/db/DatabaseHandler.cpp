//
// Created by hacketo on 19/08/16.
//

#include "DatabaseHandler.h"


errors::error DatabaseHandler::open_database() {
	int  rc;
	/* Open database */
	rc = sqlite3_open("test.db", &database);
	if( rc ){
		char buffer [50];
		sprintf(buffer, "Can't open database: %s\n", sqlite3_errmsg(database));
		return errors::get_error("DatabaseHandler", errors::DB_CANT_OPEN, buffer);
	}
	return errors::get_error();
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

errors::error DatabaseHandler::create_table(char* sql){
	char *zErrMsg = 0;
	/* Create SQL statement */
	sql = "CREATE TABLE COMPANY("  \
         "ID INT PRIMARY KEY     NOT NULL," \
         "NAME           TEXT    NOT NULL," \
         "AGE            INT     NOT NULL," \
         "ADDRESS        CHAR(50)," \
         "SALARY         REAL );";

	/* Execute SQL statement */
	int  rc = sqlite3_exec(database, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		char buffer [50];
		sprintf(buffer, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return errors::get_error("DatabaseHandler", errors::DB_CANT_OPEN, buffer);
	}else{
		fprintf(stdout, "Table created successfully\n");
	}
	return errors::get_error();
}
