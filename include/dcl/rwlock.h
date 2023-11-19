/*
 * rwlock.h
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

#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include <dcl/exception.h>

namespace dbp {

//! rwlock exception
/*!
	This class is used to signal about errors in rwlock class.
*/
class rwlock_exception: public dbp::exception {
public:
	rwlock_exception(const std::string &msg = "") noexcept:
	  dbp::exception(msg) { }
};

class rwlock_int {
	friend class rwlock;
	friend class event;
public:
	virtual ~rwlock_int() { }
	virtual void enter_read() = 0;
	virtual bool try_enter_read() = 0;
	virtual void enter_write() = 0;
	virtual bool try_enter_write() = 0;
	virtual void leave_read() = 0;
	virtual void leave_write() = 0;
};

//! Mutually exclusive lock
/*!
	This class represents the thread syncronization object, rwlock. It is
	used to prevent access of the resource by executing threads in parallel,
	providing the atomicy of the code.
*/
class rwlock: public rwlock_int {
	friend class event;
public:
	//! Constructor
	rwlock();
	//! Destructor
	virtual ~rwlock();
	//! Enter the protected area for reader
	virtual void enter_read();
	//! Try to enter the protected area for reader
	virtual bool try_enter_read();
	//! Enter the protected area for writer
	virtual void enter_write();
	//! Try to enter the protected area for writer
	virtual bool try_enter_write();
	//! Leave the protected area for reader
	virtual void leave_read();
	//! Leave the protected area for writer
	virtual void leave_write();
private:
	rwlock_int *pimpl;
};

//! rwlock reader smart pointer
/*!
	This is a special smart pointer that implements the rwlock lock. On create,
	it enters the protected area and blocks all writer threads from parallel
	executing the code protected by this quard. On destroy (deleting object
	from the stack), the lock is released.
*/
class rwlock_guard_read {
public:
	//! Constructor
	/*!
		Enter the protected area.
	*/
	rwlock_guard_read(rwlock&);
	//! Destructor
	/*!
		Leave the protected area.
	*/
	virtual ~rwlock_guard_read();
private:
	rwlock &_rwlock;
};

//! rwlock writer smart pointer
/*!
	This is a special smart pointer that implements the rwlock lock. On create,
	it enters the protected area and blocks all reader and writer threads
	from parallel executing the code protected by this quard.
	On destroy (deleting object from the stack), the lock is released.
*/
class rwlock_guard_write {
public:
	//! Constructor
	/*!
		Enter the protected area.
	*/
	rwlock_guard_write(rwlock&);
	//! Destructor
	/*!
		Leave the protected area.
	*/
	virtual ~rwlock_guard_write();
private:
	rwlock &_rwlock;
};

} //namespace

#endif /*_RWLOCK_H_*/

