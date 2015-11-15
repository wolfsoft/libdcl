/*
 * thread_impl.cpp
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

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include "dcl/strutils.h"
#include "dcl/thread.h"

namespace dbp {
namespace local {

class thread_impl: public thread_int {
public:
	thread_impl() {	};
	virtual void start();
	virtual void stop();
	virtual void pause();
	virtual void resume();
	virtual void on_execute(thread_int::execute_handler handler);
	virtual void wait_for();
private:
	pthread_t t;
	static void* execute(void *impl);
	thread_int::execute_handler _handler;
};


void thread_impl::start() {
	switch (pthread_create(&t, 0, execute, this)) {
		case 0:
			break;
		case EAGAIN:
			throw thread_exception(
			  _("The system lacked the necessary resources to create "
			  "another thread, or the system-imposed limit on the total "
			  "number of threads in a process would be exceeded"));
			break;
		case EINVAL:
			throw thread_exception(
			  _("Can't create detached thread"));
			break;
		case EPERM:
			throw thread_exception(
			  _("The caller does not have appropriate permission "
			  "to set the required scheduling parameters or "
			  "scheduling policy"));
			break;
		default:
			throw thread_exception(
			  _("pthread_create() fails by unknown error"));
	}
}

void thread_impl::stop() {
	pthread_cancel(t);
}

void thread_impl::pause() {
	pthread_kill(t, SIGSTOP);
}

void thread_impl::resume() {
	pthread_kill(t, SIGCONT);
}

void thread_impl::on_execute(thread_int::execute_handler handler) {
	_handler = handler;
}

void* thread_impl::execute(void *impl) {
	thread_impl *tr = static_cast<thread_impl*>(impl);
	if (tr->_handler)
		tr->_handler(*tr);
	pthread_exit(0);
}

void thread_impl::wait_for() {
	pthread_join(t, 0);
}

}} // namespace

