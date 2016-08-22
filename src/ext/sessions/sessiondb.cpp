//
// Created by hacketo on 20/08/16.
//

#include "sessiondb.h"
#include "../modules/sessionmanager.h"
#include "../debug.h"


SessionDB::SessionDB() : DbHandler(){};


errors::error SessionDB::open_database() {


	errors::error e = DbHandler::open_database("sessions");

	bool get_id_fromdb = true;

	if (e.code == errors::DB_NEED_CREATE_TABLE) {

		const char *sql = "CREATE TABLE SESSIONS("  \
            	"ID Int64 PRIMARY KEY     NOT NULL," \
            	"SID          CHAR(36)    	NOT NULL," \
            	"HEADER       TEXT NOT NULL," \
            	"IP        	  CHAR(25) NOT NULL," \
            	"DATA         TEXT NOT NULL ,"\
            	"START        TEXT NOT NULL ,"\
            	"END          TEXT "\
			") WITHOUT ROWID;";

		e = exec(sql);
		get_id_fromdb = false;
	}


	if (!e) {

		const char *lstId = "SELECT MAX(ID) FROM SESSIONS";
		stmtLastId = make_statement(lstId);

		const char *insertSql = "INSERT INTO SESSIONS (ID,SID,HEADER,IP,DATA,START,END) VALUES(:id,:sid,:headers,:ip,:data,:start,:end);";
		stmtSaveClient = make_statement(insertSql);


		int rc;
		if (get_id_fromdb) {
			rc = sqlite3_step(stmtLastId);

			if (rc == SQLITE_ROW) {
				indexSessionSave = sqlite3_column_int64(stmtLastId, 0) + 1;
			}
			else{
				error = true;
			}
			sqlite3_reset(stmtLastId);
		} else {
			indexSessionSave = 0;
		}
	}
	else{
		error = true;
	}

	return e;
}

errors::error SessionDB::saveSession(Session * session, bool ended){
	errors::error e;

	const char *header = session->getHeader();
	const char *sessionId = session->getSessionID();

	std::string ip = session->getIP();
	std::string data = session->getJSONData();
	std::string start_time = session->getStartTime();
	std::string end_time = ended ? session->getEndTime() : "";

	//debug::print("ip:",ip,", data:",data,", start_time:",start_time,", end_time:",end_time);

	sqlite3_bind_int(stmtSaveClient, 1, indexSessionSave);
	sqlite3_bind_text(stmtSaveClient, 2, sessionId, strlen(sessionId), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 3, header, strlen(header), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 4, ip.c_str(), ip.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 5, data.c_str(), data.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 6, start_time.c_str(), start_time.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 7, end_time.c_str(), end_time.length(), SQLITE_TRANSIENT);

	int r = sqlite3_step(stmtSaveClient);
	if (r != SQLITE_DONE) {
		e = errors::get_error("DbHandler", errors::SQL_ERROR, "Can't insert session : %s\n", get_dberror());
	}
	else{
		indexSessionSave += 1;
	}
	sqlite3_reset(stmtSaveClient);

	return e;
}

errors::error SessionDB::close(){
	errors::error e;

	sqlite3_finalize(stmtLastId);
	sqlite3_finalize(stmtSaveClient);

	DbHandler::close();

	return e;
}