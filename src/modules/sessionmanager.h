//
// Created by hacketo on 19/08/16.
//

#ifndef SERVER_SESSIONMANAGER_H
#define SERVER_SESSIONMANAGER_H

#include <boost/uuid/random_generator.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "base_module.h"
#include "../util/worker.h"

class SessionManager;
class Session;

class InvalidateSessionsWorker : public Worker<u_int32_t>{
public:
	typedef std::unique_ptr<InvalidateSessionsWorker> unique_ptr;
	static unique_ptr create(SessionManager* manager, size_t size = 50){
		return unique_ptr(new InvalidateSessionsWorker(manager, size));
	}
	InvalidateSessionsWorker(SessionManager* manager,size_t size = 50);
	void init_job_thread();
private:
	SessionManager* manager;
	void job();
};


class SessionManager {
	friend class InvalidateSessionsWorker;
public:

	typedef std::map<std::string, Session*>::iterator session_iterator;

	SessionManager();

	bool start_session(Client* client);
	bool close_session(Client* client);

	bool alive;

private:

	InvalidateSessionsWorker::unique_ptr invalidateSessionWorker;

	std::map<std::string, Session*> sessions;

	boost::uuids::random_generator generator;
};


class Session {
	friend class InvalidateSessionsWorker;
	friend class SessionManager;
public:
	typedef std::unique_ptr<Session> u_ptr;

	Session(SessionManager* manager);

	void setInt(std::string key, int value);
	void setString(std::string key, std::string value);
	void setBool(std::string key, bool value);
	void setFloat(std::string key, float value);
	void setObject(std::string key, ObjectValue* value);
	void setArray(std::string key, ArrayValue* value);


	void setHeader(std::string value);

	long elapsed();

private:

	void close();

	bool ended;
	std::string sessionId;
	const char *header;

	boost::posix_time::ptime start_time;
	boost::posix_time::ptime end_time;

	ObjectValue::u_ptr data;

	SessionManager* manager;


};


#endif //SERVER_SESSIONMANAGER_H
