//
// Created by hacketo on 19/08/16.
//
#include "cfg/config.h"

#include "ext/sessions/sessionmanager.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "client/client.h"
#include "debug.h"
#include "util/date.h"

using namespace protocol::packet;

namespace ext {
namespace sessions {

InvalidateSessionsWorker::InvalidateSessionsWorker(SessionManager* manager) :
		Worker(), manager(manager), sessionDB(&manager->sessionDB){


}
error::code InvalidateSessionsWorker::init(){
	error::code e = sessionDB->open_database();

	if (!e) {
		m_thread = boost::thread(&InvalidateSessionsWorker::job, this);
	}
	return e;
}

//todo: transform en async_call(invalidate_session, config::SESSION_TIME)
void InvalidateSessionsWorker::job(){
	while(!m_interrupted.load() && manager->alive) {
		std::map<std::string, Session::u_ptr> &sessions = manager->sessions;
		std::vector<std::string> sessionToInvalidate;



		for (SessionManager::session_iterator it = sessions.begin(); it != sessions.end(); ++it) {
			Session *s = it->second.get();
			if (s->ended && s->elapsed() > config::SESSION_TIME) {
				sessionToInvalidate.push_back(it->first);
			}
		}

		if (sessionToInvalidate.size() > 0) {
			// save de la session en bdd
			unsigned long i = 0, len = sessionToInvalidate.size();
			for (; i < len; ++i) {
				sessionDB->saveSession(sessions[sessionToInvalidate.at(i)].get(), true);
			}
			//Supression de la session
			for (i = 0; i < len; ++i) {
				sessions.erase(sessionToInvalidate.at(i));
			}
			sessionToInvalidate.clear();
		}

		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	}

	DEBUG_PRINT("SessionManager InvalidateSessionsWorker ended");
}

const std::string SessionManager::COOKIE_NAME = "ws_sid";

SessionManager::SessionManager() : alive(true){
	invalidateSessionWorker = InvalidateSessionsWorker::create(this);
	error::code e = invalidateSessionWorker->init();

	if (e){
		DEBUG_PRINT("Can't start SessionManager value : ",e.msg);
		alive = false;
	}

}
void SessionManager::start_session(Client* client, protocol::http::header* handshake, error::code& error ){
	//Create session / id

	if (handshake->cookies.count(COOKIE_NAME)) {
		const char *sessId = handshake->cookies[COOKIE_NAME].c_str();

		if (sessions.count(sessId)){
			client->session = sessions[sessId].get();
			client->session->client = client;

			DEBUG_PRINT("Resume Session : ",client->get_id()," - ",sessId);
			sessionDB.saveSession(client->session, false);

			client->session->reopen(handshake);
			return;
		}
	}
	Session *s = new Session(this, client, handshake);

	boost::uuids::uuid u = generator();
	s->sessionId = boost::lexical_cast<std::string>(u).c_str();

	client->session = s;
	sessions[s->sessionId] = Session::u_ptr(s);
	DEBUG_PRINT("Create Session : ",client->get_id()," - ",s->sessionId);
}

void SessionManager::update_handshake(Client* client, protocol::http::header* handshake, error::code& error ) {
	if (client->session->updateCookie) {
		protocol::http::add_cookie(handshake, COOKIE_NAME, client->session->sessionId, config::SESSION_TIME/1000);

		client->session->updateCookie = false;
	}
}

bool SessionManager::close_session(Client* client){
	// save session
	if (client->session) {
		DEBUG_PRINT("Close session : ", client->get_id(), " - ", client->session->getSessionID());
		client->session->close();
	}

}


Session::Session(SessionManager* manager, Client* client,protocol::http::header* handshake) :
		manager(manager), client(client), ended(false), updateCookie(true), handshake(handshake) {
	data = ObjectValue::u_ptr(new ObjectValue);
	start_time = boost::posix_time::microsec_clock::local_time();
}

Session::~Session(){
	delete (handshake);
}

long Session::elapsed(){
	boost::posix_time::time_duration diff;
	if (ended){
		diff = boost::posix_time::microsec_clock::local_time() - end_time;
	}
	else{
		diff = boost::posix_time::microsec_clock::local_time() - start_time;
	}
	return diff.total_milliseconds();
}

void Session::setInt(std::string& key, int value){
	data->set(key, new IntValue(value));
}
void Session::setString(std::string& key, std::string value){
	data->set(key, new StringValue(value));
}
void Session::setBool(std::string& key, bool value){
	data->set(key, new BoolValue(value));
}
void Session::setFloat(std::string& key, float value){
	data->set(key, new FloatValue(value));
}
void Session::setObject(std::string& key, ObjectValue* value){
	data->set(key, value);
}
void Session::setArray(std::string& key, ArrayValue* value){
	data->set(key, value);
}


std::string Session::getStartTime(){
	return date::gmt(start_time);
}
std::string Session::getEndTime(){
	return date::gmt(end_time);
}

void Session::close() {

	end_time = boost::posix_time::microsec_clock::local_time();
	ended = true;
}

void Session::reopen(protocol::http::header* handshake) {
	ended = false;
	start_time = boost::posix_time::microsec_clock::local_time();
	end_time = boost::posix_time::ptime();

	delete(this->handshake);
	this->handshake = handshake;
}

const char * Session::getHeader() {
	return handshake->headers_raw.c_str();
}
const char * Session::getSessionID() {
	return sessionId.c_str();
}

std::string Session::getIP(){
	return client->get_ip();
}

std::string Session::getJSONData(){
	return GenericValue::toJSON(data.get());
}


SessionDB::SessionDB() : DbHandler() {};


error::code SessionDB::open_database() {


	error::code e = DbHandler::open_database("sessions");

	bool get_id_fromdb = true;

	if (e.value == error::DB_NEED_CREATE_TABLE) {

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
			} else {
				error = true;
			}
			sqlite3_reset(stmtLastId);
		} else {
			indexSessionSave = 0;
		}
	} else {
		error = true;
	}

	return e;
}

error::code SessionDB::saveSession(Session *session, bool ended) {
	error::code e;

	const char *header = session->getHeader();
	const char *sessionId = session->getSessionID();

	std::string ip = session->getIP();
	std::string data = session->getJSONData();
	std::string start_time = session->getStartTime();
	std::string end_time = ended ? session->getEndTime() : "";

	//DEBUG_PRINT("ip:",ip,", data:",data,", start_time:",start_time,", end_time:",end_time);

	sqlite3_bind_int(stmtSaveClient, 1, indexSessionSave);
	sqlite3_bind_text(stmtSaveClient, 2, sessionId, strlen(sessionId), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 3, header, strlen(header), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 4, ip.c_str(), ip.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 5, data.c_str(), data.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 6, start_time.c_str(), start_time.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmtSaveClient, 7, end_time.c_str(), end_time.length(), SQLITE_TRANSIENT);

	int r = sqlite3_step(stmtSaveClient);
	if (r != SQLITE_DONE) {
		error::get_code(e, "DbHandler", error::SQL_ERROR, "Can't insert session : %s\n", get_dberror());
	} else {
		indexSessionSave += 1;
	}
	sqlite3_reset(stmtSaveClient);

	return e;
}

error::code SessionDB::close() {
	error::code e;

	sqlite3_finalize(stmtLastId);
	sqlite3_finalize(stmtSaveClient);

	DbHandler::close();

	return e;
}

}
}