//
// Created by hacketo on 22/05/15.
//

#ifndef WS_SERVER_SYNCHRONIZEDPOOL_H
#define WS_SERVER_SYNCHRONIZEDPOOL_H

#include <algorithm>
#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/call_traits.hpp>
#include <boost/bind.hpp>

#include <boost/timer/timer.hpp> // for auto_cpu_timer

template<class T>
class SafeDeQue {
public:

	typedef boost::circular_buffer<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

// `param_type` represents the "best" way to pass a parameter of type `value_type` to a method.
	explicit SafeDeQue(size_type capacity) : m_unread(0), m_container(capacity), interrupted(false) , wait_end(false){}

	/**
	 * Méthode d'ajout d'un item dans la liste
	 * Thread safe, peut bloqué tant que l'action d'ajout n'est pas terminée
	 * @return true si la ressource à été insérée, sinon check interrupt
	 */
	bool push(param_type item) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&SafeDeQue<value_type>::cond_is_not_full, this));

		if (interrupted) {
			return false;
		}

		m_container.push_front(item);
		++m_unread;
		lock.unlock();
		m_not_empty.notify_one();
		return true;
	}

	/**
	 * Méthode pop d'un item dans la liste
	 * Thread safe, peut bloqué tant que l'action d'ajout n'est pas terminée
	 * @return true si la ressource à été récupérée, sinon check interrupt
	 */
	bool pop(value_type &pItem) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_empty.wait(lock, boost::bind(&SafeDeQue<value_type>::cond_is_not_empty, this));
		if (interrupted && !(wait_end && is_not_empty())) {
			return false;
		}
		pItem = m_container[--m_unread];
		lock.unlock();
		m_not_full.notify_one();
		return true;
	}

	size_type weight(){
		return m_unread;
	}


	void interrupt(bool wait_the_end = false){
		if (!interrupted) {
			interrupted = true;
			wait_end = wait_the_end;
			m_not_empty.notify_all();
			m_not_full.notify_all();
		}
	}



	bool is_not_empty() const { return m_unread > 0; }
	bool is_not_full() const { return m_unread < m_container.capacity(); }

protected:
	bool cond_is_not_empty() const { return interrupted || m_unread > 0; }
	bool cond_is_not_full() const { return interrupted || m_unread < m_container.capacity(); }

private:
	SafeDeQue(const SafeDeQue &) {};              // Disabled copy constructor.
	SafeDeQue &operator=(const SafeDeQue &) {}; // Disabled assign operator.

	bool interrupted;
	bool wait_end;

	size_type m_unread;
	container_type m_container;
	boost::mutex m_mutex;

	boost::lock_guard<boost::mutex>* m_lock_waitone;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};


#endif //WS_SERVER_SYNCHRONIZEDPOOL_H
