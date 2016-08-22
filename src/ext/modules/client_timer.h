//
// Created by hacketo on 14/08/16.
//

#ifndef SERVER_CLIENT_TIMER_H
#define SERVER_CLIENT_TIMER_H

#include "../client/client.h"

/**
 * Utilisé pour avoir un modèle d'attente de réponse d'un client
 *
 * http://www.boost.org/doc/libs/1_50_0/doc/html/boost_asio/reference/basic_deadline_timer.html
 *
 * Le timer peut avoir 3 status
 * INIT     : le timer à été initialisé
 * STARTED  : le timer à été démarré
 * ENDED    : le timer à fini sans avoir de réponse du client
 * CLIENT_RESPONDED : le client à répondu, le timer est invalidé
 *
 */
class Client_timer {

public:
	enum Status {
		INIT, STARTED, ENDED, CLIENT_RESPONDED
	};


	Client_timer(Client *client, uint64_t time_to_wait);

	/**
	 * Démarre le timer, passe le status à STARTED
	 */
	virtual void start_waiting();

	/**
	 * Retourne le temps passé depuis le start_time si le client n'as pas répondu
	 * Retourne le temps que le client a mis à répondre si il à répondu
	 * @return
	 */
	int elapsed() const;

	/**
	 * Test si le timer est fini , passe le status à ENDED si besoin
	 * Soit fini par le temps, soit le client à répondu
	 * @return
	 */
	bool is_at_the_end();

	/**
	 * True si le client à répondu
	 * @return bool
	 */
	bool did_client_respond();

	/**
	 * Tick de réponse de la part du client, passe le status à CLIENT_RESPONDED
	 * Si le timer est déjà fini ne fait rien
	 * @return bool True si le status est passé à CLIENT_RESPONDED, False si le timer était déjà fini
	 */
	virtual bool client_response();

protected:
	boost::posix_time::ptime start_time;
	boost::posix_time::ptime end_time;

	/**
	 * temps en milliseconde
	 */
	uint64_t time_to_wait;

	Client::s_ptr client;

	/**
	 * statut du timer
	 */
	Status status;
};

/**
 * Classe de timer asynchrone
 * Possibilité de binder un callback lorsque le client répond ou le timer se fini
 *  type : std::function<void(bool,int)>
 */
class Client_timer_async : public Client_timer {

	struct TimerData{
		boost::asio::deadline_timer timer;
		TimerData(boost::asio::io_service& ioService, boost::posix_time::time_duration time) : timer(ioService, time){};
	};

public:
	typedef std::function<void(bool,int)> callback_signature;

	Client_timer_async(Client* client, uint64_t time_to_wait, callback_signature func);
	virtual void start_waiting();
	virtual bool client_response();
	void job();
private:
	boost::asio::io_service io;
	TimerData* data;
	callback_signature callback;
};


#endif //SERVER_CLIENT_TIMER_H
