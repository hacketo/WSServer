//
// Created by hacketo on 19/08/16.
//

#ifndef SERVER_SESSIONMANAGER_H
#define SERVER_SESSIONMANAGER_H

#include <boost/uuid/random_generator.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "util/worker.h"
#include "protocol/http.h"
#include "util/genericvalue.h"
#include "db/dbhandler.h"

class Client;

namespace ext {
namespace sessions {

	class SessionManager;
	class InvalidateSessionsWorker;

	class Session {
		friend class InvalidateSessionsWorker;

		friend class SessionManager;

		friend class Client;

	public:
		typedef boost::shared_ptr<Session> s_ptr;
		typedef std::unique_ptr<Session> u_ptr;

		Session(SessionManager *manager, Client *client, protocol::http::handshake *handshake);

		~Session();

		void setInt(std::string& key, int value);

		void setString(std::string& key, std::string value);

		void setBool(std::string& key, bool value);

		void setFloat(std::string& key, float value);

		void setObject(std::string& key, ObjectValue *value);

		void setArray(std::string& key, ArrayValue *value);

		std::string getStartTime();

		std::string getEndTime();

		const char *getHeader();

		const char *getSessionID();

		std::string getIP();

		std::string getJSONData();

		long elapsed();

	private:

		void close();

		void reopen(protocol::http::handshake *handshake);

		bool ended;
		bool updateCookie;

		std::string sessionId;
		ObjectValue::u_ptr data;
		protocol::http::handshake *handshake;

		boost::posix_time::ptime start_time;
		boost::posix_time::ptime end_time;
		SessionManager *manager;
		Client *client;
	};


	class SessionDB : public DbHandler {
	public:
		SessionDB();
		errors::error_code open_database();
		errors::error_code saveSession(Session *pSession, bool ended);
		errors::error_code close();

	private:
		uint64_t indexSessionSave;
		sqlite3_stmt *stmtSaveClient;
		sqlite3_stmt *stmtLastId;
	};


	class InvalidateSessionsWorker : public Worker<u_int32_t> {
	public:
		typedef std::unique_ptr<InvalidateSessionsWorker> u_ptr;

		static u_ptr create(SessionManager *manager, size_t size = 50) {
			return u_ptr(new InvalidateSessionsWorker(manager, size));
		}

		InvalidateSessionsWorker(SessionManager *manager, size_t size = 50);

		errors::error_code init_job_thread();

	private:

		SessionDB *sessionDB;
		SessionManager *manager;

		void job();
	};


	class SessionManager {
		friend class InvalidateSessionsWorker;

	public:
		typedef std::unique_ptr<SessionManager> u_ptr;
		typedef std::map<std::string, Session::u_ptr>::iterator session_iterator;

		static const std::string COOKIE_NAME;

		SessionManager();

		void start_session(Client *client, protocol::http::handshake *handshake, errors::error_code &error);

		void update_handshake(Client *client, protocol::http::handshake *handshake, errors::error_code &error);

		bool close_session(Client *client);

		bool alive;

	private:

		SessionDB sessionDB;

		InvalidateSessionsWorker::u_ptr invalidateSessionWorker;

		std::map<std::string, Session::u_ptr> sessions;

		boost::uuids::random_generator generator;
	};

}
}
#endif //SERVER_SESSIONMANAGER_H
