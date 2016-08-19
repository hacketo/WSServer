//
// Created by hacketo on 19/08/16.
//

#include "sessionmanager.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "../client/client.h"
#include "../debug.h"

using namespace protocol::packet;

InvalidateSessionsWorker::InvalidateSessionsWorker(SessionManager* manager, size_t size) :
		Worker<u_int32_t>(size), manager(manager){
}
void InvalidateSessionsWorker::init_job_thread(){
	worker = boost::thread(&InvalidateSessionsWorker::job, this);
}


void InvalidateSessionsWorker::job(){
	while(manager->alive) {
		std::map<std::string, Session *> &sessions = manager->sessions;
		std::vector<std::string> sessionToInvalidate;
		for (std::map<std::string, Session *>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
			Session *s = it->second;
			if (s->ended && s->elapsed() > config::SESSION_TIME) {
				sessionToInvalidate.push_back(it->first);
			}
		}
		unsigned long i = 0, len = sessionToInvalidate.size();
		for (; i < len; ++i) {
			// save de la session en bdd
			//sessions.erase(sessionToInvalidate.at(i));
		}

		for (i = 0; i < len; ++i) {
			sessions.erase(sessionToInvalidate.at(i));
		}
		sessionToInvalidate.clear();

		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	}
	debug::print("SessionManager InvalidateSessionsWorker ended");
}

SessionManager::SessionManager() : alive(true){
	invalidateSessionWorker = InvalidateSessionsWorker::create(this);
	invalidateSessionWorker->init_job_thread();
}
bool SessionManager::start_session(Client* client){
	//Create session / id

	Session *s = new Session(this);

	boost::uuids::uuid u = generator();
	s->sessionId = boost::lexical_cast<std::string>(u);

	client->session = Session::u_ptr(s);
	sessions[s->sessionId] = s;
	return true;
}


bool SessionManager::close_session(Client* client){
	// save session

	client->session->close();

}


Session::Session(SessionManager* manager) : manager(manager), ended(false) {
	data = ObjectValue::u_ptr(new ObjectValue);
	start_time = boost::posix_time::microsec_clock::local_time();
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
void Session::setHeader(std::string value){
	header = value.c_str();
}

void Session::close() {
	end_time = boost::posix_time::microsec_clock::local_time();
	ended = true;
}
