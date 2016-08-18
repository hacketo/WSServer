//
// Created by hacketo on 14/08/16.
//

#include "client_timer.h"

Client_timer::Client_timer(Client *client, uint64_t time_to_wait):
	client(Client::s_ptr(client)),
	time_to_wait(time_to_wait),
	status(INIT){

}

void Client_timer::start_waiting(){
	start_time = boost::posix_time::microsec_clock::local_time();
	status = STARTED;
}

int Client_timer::elapsed() const{
	boost::posix_time::time_duration diff;
	if(status < CLIENT_RESPONDED) {
		diff = boost::posix_time::microsec_clock::local_time() - start_time;
	}
	else{
		diff = end_time - start_time;
	}
	return diff.total_milliseconds();
}

bool Client_timer::is_at_the_end(){
	// Le timer est fini ..
	if(status > STARTED){
		return true;
	}
	// Le timer n'est pas fini, calcul du temp passé
	if(elapsed() > time_to_wait) {
		status = ENDED;
	}
	// Check d'un possible nouveau status
	return status > STARTED;
}


bool Client_timer::did_client_respond(){
	return status == CLIENT_RESPONDED;
}

bool Client_timer::client_response(){
	if (status == STARTED) {
		end_time = boost::posix_time::microsec_clock::local_time();
		status = CLIENT_RESPONDED;
		return true;
	}
	return false;
}



Client_timer_async::Client_timer_async(Client* client, uint64_t time_to_wait, callback_signature func):
	Client_timer(client,time_to_wait), callback(func){

}
void Client_timer_async::start_waiting(){
	Client_timer::start_waiting();

	data = new TimerData(io, boost::posix_time::millisec(time_to_wait / 1000));
	data->timer.async_wait(boost::bind(&Client_timer_async::job, this));

}

bool Client_timer_async::client_response(){
	if (Client_timer::client_response()) {
		data->timer.cancel();
		callback(true, elapsed());
		return true;
	}
	return false;
}

void Client_timer_async::job(){
	if (status == STARTED) {
		callback(false, elapsed());
	}
}

