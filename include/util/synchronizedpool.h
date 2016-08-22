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
class SynchronizedPool {
public:

	typedef boost::circular_buffer<T> container_type;
	typedef typename container_type::size_type size_type;
	typedef typename container_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

// `param_type` represents the "best" way to pass a parameter of type `value_type` to a method.
	explicit SynchronizedPool(size_type capacity) : m_unread(0), m_container(capacity) {}

	/**
	 * Méthode d'ajout d'un item dans la liste
	 * Thread safe, peut bloqué tant que l'action d'ajout n'est pas terminée
	 * @param item
	 */
	void push_front(param_type item) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&SynchronizedPool<value_type>::is_not_full, this));
		m_container.push_front(item);
		++m_unread;
		lock.unlock();
		m_not_empty.notify_one();
	}

	/**
	 * Méthode pop d'un item dans la liste
	 * Thread safe, peut bloqué tant que l'action d'ajout n'est pas terminée
	 * @param item
	 */
	void pop_back(value_type *pItem) {
		boost::mutex::scoped_lock lock(m_mutex);
		m_not_empty.wait(lock, boost::bind(&SynchronizedPool<value_type>::is_not_empty, this));
		*pItem = m_container[--m_unread];
		lock.unlock();
		m_not_full.notify_one();
	}

	bool is_not_empty() const { return m_unread > 0; }

	bool is_not_full() const { return m_unread < m_container.capacity(); }

private:
	SynchronizedPool(const SynchronizedPool &) {};              // Disabled copy constructor.
	SynchronizedPool &operator=(const SynchronizedPool &) {}; // Disabled assign operator.

	size_type m_unread;
	container_type m_container;
	boost::mutex m_mutex;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};


#endif //WS_SERVER_SYNCHRONIZEDPOOL_H
