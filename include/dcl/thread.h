/*
 * thread.h
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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <dcl/exception.h>
#include <dcl/delegate.h>

namespace dbp {

//! Thread exception
/*!
	This exception class is raised on errors in thread class.
*/
class thread_exception: public exception {
public:
	thread_exception(const std::string &msg): exception(msg) { }
};

//! Thread interface
/*!
	This is an interface to the thread class. Any concrete thread class
	should subclass from this and implement pure virtual methods.
*/
class thread_int {
public:
	//! Entry point execute handler
	typedef delegate1<thread_int&,void> execute_handler;
	//! Destructor
	virtual ~thread_int() { };
	//! Start the thread execution
	virtual void start() = 0;
	//! Stop the thread execution
	virtual void stop() = 0;
	//! Pause the thread execution
	virtual void pause() = 0;
	//! Resume of the thread execution
	virtual void resume() = 0;
	//! Register the entry point execute handler
	/*!
		You need to register your own thread execution handler. When the
		thread starts, your execution handler will be used as entry point
		of the thread, so this is a place to put your code to make the
		thread to do something useful.

		See the 'examples/threads' directory for the sample code.

		\param handler an execute handler delegate.
	*/
	virtual void on_execute(execute_handler handler) = 0;
	//! Wait the thread for execution complete
	virtual void wait_for() = 0;
};

//! System thread
/*!
	This class represents the system thread. The thread is concurrently running
	task, executed with your other code in parallel.
*/
class thread: public thread_int {
public:
	//! Constructor
	thread();
	//! Constructor
	thread(const thread &src);
	//! Copy operator
	thread& operator=(const thread &src) {
		on_execute(src._handler);
		return *this;
	}
	virtual ~thread();
	virtual void start();
	virtual void stop();
	virtual void pause();
	virtual void resume();
	virtual void on_execute(thread_int::execute_handler handler);
	virtual void wait_for();
private:
	thread_int::execute_handler _handler;
	thread_int *pimpl;
};

}

#endif /*_THREAD_H_*/
