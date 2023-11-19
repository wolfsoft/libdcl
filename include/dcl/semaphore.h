/*
 * semaphore.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2009 Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#include <dcl/exception.h>

namespace dbp {

//! Semaphore exception class
/*!
	This class is used to signal about errors in semaphore class.
*/
class semaphore_exception: public dbp::exception {
public:
	semaphore_exception(const std::string &msg = "") noexcept:
	  dbp::exception(msg) { }
};

class semaphore_int {
public:
	virtual ~semaphore_int() { }
	virtual void lock() = 0;
	virtual bool trylock() = 0;
	virtual void unlock() = 0;
};

//! Semaphore synchronization class
/*!
	This class represents the thread syncronization object, semaphore. It is
	frequently used in the multithreaded producer-consumer algorithms.
*/
class semaphore: public semaphore_int {
public:
	//! Constructor
	semaphore(int initial_value = 0);
	//! Destructor
	virtual ~semaphore();
	//! Lock the semaphore
	/*!
		lock() decrements the internal lock counter. If the lock counter is
		greater than zero, then the decrement proceeds, and the function
		returns immediately. If the lock counter is zero, then the call blocks
		calling thread until the lock counter rises above zero.
	*/
	virtual void lock();
	//! Try to lock the semaphore
	/*!
		trylock() decrements the internal lock counter. If the lock counter is
		greater than zero, then the decrement proceeds, and the function
		returns true immediately. If the lock counter is zero, then the
		function returns false without blocking.
	*/
	virtual bool trylock();
	//! Unlock the semaphore
	/*!
		unlock() increments the internal lock counter. If the lock counter
		consequently becomes greater than zero, then another thread blocked
		in the lock() call will be woken up and proceed to lock the semaphore.
	*/
	virtual void unlock();
private:
	semaphore_int *pimpl;
};

} //namespace

#endif /*__SEMAPHORE_H_*/

