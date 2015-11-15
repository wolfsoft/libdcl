/*
 * pool.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
 *
 * DCL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * DCL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DCL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef _POOL_H_
#define _POOL_H_

#include <algorithm>
#include <queue>
#include <map>
#include <memory>

#include <dcl/mutex.h>
#include <dcl/semaphore.h>
#include <dcl/noncopyable.h>

namespace dbp {

class pool_size_policy {
public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
};

class pool_size_unlimited_policy: public pool_size_policy {
public:
	virtual void lock() { };
	virtual void unlock() { };
};

class pool_size_limited_policy: public pool_size_policy {
public:
	pool_size_limited_policy(size_t size_limit): s(size_limit) { };
	virtual void lock() { s.lock(); };
	virtual void unlock() { s.unlock(); }
private:
	semaphore s;
};

template <class T>
class pool_ptr;

//!	Objects pool template class
/*!
	This is a specialized object factory class with caching feature. When
	acquiring an object, the factory checks the pool for the not acquired one;
	if none found, the factory creates one.

	When the object is not needed by caller, it is returning into the pool.
	So, this pool class preserving time for the object creation if the object
	constructing or initializing is expensive.
*/
template <class T>
class pool: public noncopyable {
	friend class pool_ptr<T>;
public:
	pool() {
		size_policy.reset(new pool_size_unlimited_policy());
	}
	pool(std::auto_ptr<pool_size_policy> ps): size_policy(ps) { }
	//! Destructor
	virtual ~pool() {
		std::for_each(_all.begin(), _all.end(), delete_item);
	}
	//! Acquire an object
	/*!
		Create the object instance if the instance is not found in the
		pool or return the instance from the pool otherwise.
		
		\returns the smart pointer to the object instance
	*/
	pool_ptr<T> acquire(const std::string &key = std::string("")) {
		return pool_ptr<T>(acquire_item(key), this, key);
	}
	//! Get current pool size
	int size() const {
		return _all.size();
	}
	//! Get the number of cached entries
	int in_use() const {
		int total = 0;
		for (queue_idx_iterator it = _free.begin();
		  it != _free.end(); ++it) {
			total += it->second.size();
		}
		return _all.size() - total;
	}
private:
	typedef std::map<std::string, std::queue<T*> > queue_idx;
	typedef typename queue_idx::const_iterator queue_idx_iterator;
	queue_idx _free;
	std::vector<T*> _all;
	mutex cs;
	std::auto_ptr<pool_size_policy> size_policy;
	T* acquire_item(const std::string &key) {
		size_policy->lock();
		mutex_guard guard(cs);
		std::queue<T*> &q = _free[key];
		if (q.empty()) {
			T *item = new T();
			_all.push_back(item);
			return item;
		}
		T *item = q.front();
		q.pop();
		return item;
	}
	void release_item(T *item, const std::string &key) {
		size_policy->unlock();
		mutex_guard guard(cs);
		std::queue<T*> &q = _free[key];
		q.push(item);
	}
	static void delete_item(T *item) {
		delete item;
	}
};

//! Pool item smart pointer
/*!
	This class is a smart pointer. The dbp::pool class is returning this
	pointer	when acquiring an object from the pool. When the pointer is
	destroyed (from the stack), the object it referenced to is returned
	into the pool.
*/
template <class T>
class pool_ptr {
	friend class pool<T>;
public:
	pool_ptr(): _item(NULL), _pool(NULL) { }
	pool_ptr(T *item, pool<T> *pool, const std::string &key): _item(item),
	  _pool(pool), _key(key) { }
	//! Destructor
	~pool_ptr() {
		if (_pool && _item) {
			_pool->release_item(_item, _key);
		}
	}
	pool_ptr(const pool_ptr<T> &src): _item(src._item), _pool(src._pool),
	  _key(src._key) {
		const_cast<pool_ptr<T>&>(src)._pool = NULL;
		const_cast<pool_ptr<T>&>(src)._item = NULL;
	}
	//! Assignment operator
	pool_ptr<T>& operator=(const pool_ptr<T> &src) {
		if (_pool && _item) {
			_pool->release_item(_item, _key);
		}
		_item = src._item;
		_pool = src._pool;
		_key = src._key;
		const_cast<pool_ptr<T>&>(src)._pool = NULL;
		const_cast<pool_ptr<T>&>(src)._item = NULL;
		return *this;
	}
	//! Return the object reference
	T& operator*() const {
		return *_item;
	}
	//! Return the pointer to the object
	T* operator->() const {
		return _item;
	}
private:
	T *_item;
	pool<T> *_pool;
	std::string _key;
};

}

#endif /*_POOL_H_*/

