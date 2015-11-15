/*
 * event.h
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

#ifndef _EVENT_H_
#define _EVENT_H_

#include <dcl/exception.h>
#include <dcl/mutex.h>

namespace dbp {

class event_int {
public:
	virtual ~event_int() { }
	virtual void wait() = 0;
	virtual void raise() = 0;
};

//! Condition variable lock (event)
/*!
	A condition variable is a synchronization object that allows threads
	to suspend execution and relinquish the processors until some predicate
	on shared data is satisfied. The basic operations on conditions are:
	signal the condition (when the predicate becomes true), and wait for
	the condition, suspending the thread execution until another thread
	signals the condition.

	A condition variable must always be associated with a mutex, to avoid
	the race condition where a thread prepares to wait on a condition variable
	and another thread signals the condition just before the first thread
	actually waits on it.
*/
class event: public event_int {
public:
	//! Constructor
	/*!
		Initializes the event with the provided mutex object.
		
		\param m the mutex to associate with.
	*/
	event(mutex &m);
	//! Destructor
	virtual ~event();
	//! Wait for event raising
	/*!
		Atomically unlocks the associated mutex and waits for the condition
		variable to be raised. The thread execution is suspended and does not
		consume any CPU time until the condition variable is raised.
		The mutex must be locked by the calling thread on entrance to wait().
		Before returning to the calling thread, wait() re-acquires mutex.
	*/
	virtual void wait();
	//! Raise the event
	/*!
		Restarts all the threads that are waiting on this condition variable.
		Nothing happens if no threads are waiting on this one.
	*/
	virtual void raise();
private:
	event_int *pimpl;
	mutex &_m;
};

} //namespace

#endif /*_EVENT_H_*/
