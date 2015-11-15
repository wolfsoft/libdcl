/*
 * thread_impl.cpp
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
 *
 * dbPager Server is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * dbPager Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dbPager Server; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <windows.h>

#include "dcl/strutils.h"
#include "dcl/thread.h"
#include "dcl/mutex.h"

namespace dbp {
namespace local {

using namespace std;

class thread_impl: public thread_int {
public:
	thread_impl(): _id(0), _handle(0) { };
	virtual ~thread_impl();
	virtual void start();
	virtual void stop();
	virtual void pause();
	virtual void resume();
	virtual void on_execute(thread_int::execute_handler handler);
	virtual void wait_for();
private:
	DWORD _id;
	HANDLE _handle;
	thread_int::execute_handler _handler;
	static DWORD WINAPI execute(LPVOID impl);
};

thread_impl::~thread_impl() {
	if (_handle)
		CloseHandle(_handle);
}

void thread_impl::start() {
	_handle = CreateThread(NULL, 0, execute, this, CREATE_SUSPENDED, &_id);
	if (!_handle) {
		throw thread_exception(
		  _("The system lacked the necessary resources to create "
		  "another thread, or the system-imposed limit on the total "
		  "number of threads in a process would be exceeded"));
	}
	resume();
}

void thread_impl::stop() {
	// however, the MSDN says that it's dangerous :(
	if (_handle)
		TerminateThread(_handle, 0);
}

void thread_impl::pause() {
	if (_handle)
		SuspendThread(_handle);
}

void thread_impl::resume() {
	if (_handle)
		ResumeThread(_handle);
}

void thread_impl::on_execute(thread_int::execute_handler handler) {
	_handler = handler;
}

DWORD WINAPI thread_impl::execute(LPVOID impl) {
	thread_impl *tr = static_cast<thread_impl*>(impl);
	if (tr->_handler)
		tr->_handler(*tr);
	return 0;
}

void thread_impl::wait_for() {
	if (_handle)
		WaitForSingleObject(_handle, INFINITE);
}

}} // namespace

