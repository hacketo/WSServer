//
// Created by hacketo on 19/08/16.
//
#include "server/config.h"

#include "ext/sessions/sessionmanager.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "client/client.h"
#include "debug.h"
#include "util/date.h"

using namespace protocol::packet;

InvalidateSessionsWorker::InvalidateSessionsWorker(SessionManager* manager, size_t size) :
		Worker<u_int32_t>(size), manager(manager), sessionDB(&manager->sessionDB){


}
errors::error InvalidateSessionsWorker::init_job_thread(){

	errors::error e = sessionDB->open_database();

	if (!e) {
		worker = boost::thread(&InvalidateSessionsWorker::job, this);
	}
	return e;
}


void InvalidateSessionsWorker::job(){
	while(manager->alive) {
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
	errors::error e = invalidateSessionWorker->init_job_thread();

	if (e){
		DEBUG_PRINT("Can't start session error : ",e.msg);
		alive = false;
	}

}
void SessionManager::start_session(Client* client, protocol::http::handshake* handshake, errors::error& error ){
	//Create session / id

	if (handshake->cookies.count(COOKIE_NAME)) {
		const char *sessId = handshake->cookies[COOKIE_NAME].c_str();

		if (sessions.count(sessId)){
			client->session = sessions[sessId].get();
			client->session->client = client;

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
}

void SessionManager::update_handshake(Client* client, protocol::http::handshake* handshake, errors::error& error ) {
	if (client->session->updateCookie) {
		http::add_cookie(handshake, COOKIE_NAME, client->session->sessionId, config::SESSION_TIME/1000);

		client->session->updateCookie = false;
	}
}

bool SessionManager::close_session(Client* client){
	// save session

	client->session->close();

}


Session::Session(SessionManager* manager, Client* client,protocol::http::handshake* handshake) :
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

void Session::setInt(std::string key, int value){
	data->set(key, new IntValue(value));
}
void Session::setString(std::string key, std::string value){
	data->set(key, new StringValue(value));
}
void Session::setBool(std::string key, bool value){
	data->set(key, new BoolValue(value));
}
void Session::setFloat(std::string key, float value){
	data->set(key, new FloatValue(value));
}
void Session::setObject(std::string key, ObjectValue* value){
	data->set(key, value);
}
void Session::setArray(std::string key, ArrayValue* value){
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

void Session::reopen(http::handshake* handshake) {
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