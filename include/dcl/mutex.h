/*
 * mutex.h
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

#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <dcl/exception.h>

namespace dbp {

//! Mutex exception
/*!
	This class is used to signal about errors in mutex class.
*/
class mutex_exception: public dbp::exception {
public:
	mutex_exception(const std::string &msg = "") throw():
	  dbp::exception(msg) { }
};

class mutex_int {
	friend class mutex;
	friend class event;
public:
	virtual ~mutex_int() { }
	virtual void enter() = 0;
	virtual bool try_enter() = 0;
	virtual void leave() = 0;
protected:
	virtual void* handle() = 0;
};

//! Mutually exclusive lock
/*!
	This class represents the thread syncronization object, mutex. It is
	used to prevent access of the resource by executing threads in parallel,
	providing the atomicy of the code.
*/
class mutex: public mutex_int {
	friend class event;
public:
	//! Constructor
	mutex();
	//! Destructor
	virtual ~mutex();
	//! Enter the protected area
	/*!
		When called, no other thread can enter the protected area. These are
		blocked and wait for releasing of this lock. If the one thread tries
		to lock same mutex twice, the exception will be thrown.
	*/
	virtual void enter();
	//! Try to enter the protected area
	/*!
		Attempts to enter a protected area without blocking.
		
		\returns true, if the protected area is successfully entered or the
		current thread already owns the mutex.
	*/
	virtual bool try_enter();
	//! Leave the protected area
	/*!
		When called, the lock is releasing; other threads continue to work
		with ptotected area. The mutex should be unlocked by the same thread
		which was lock it, or the exception will be thrown.
	*/
	virtual void leave();
protected:
	virtual void* handle();
private:
	mutex_int *pimpl;
};

//! Mutex smart pointer
/*!
	This is a special smart pointer that implements the mutex lock. On create,
	it enters the protected area and blocks other threads from parallel
	executing the code protected by this quard. On destroy (deleting object
	from the stack), the lock is released.
*/
class mutex_guard {
public:
	//! Constructor
	/*!
		Enter the protected area.
	*/
	mutex_guard(mutex&);
	//! Destructor
	/*!
		Leave the protected area.
	*/
	virtual ~mutex_guard();
private:
	mutex &_mutex;
};

} //namespace

#endif /*_MUTEX_H_*/

