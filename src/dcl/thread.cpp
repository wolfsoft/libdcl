/*
 * thread.cpp
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

#include <dcl/thread.h>

#ifdef _WIN32
#include "win32/thread_impl.cpp"
#else
#include "posix/thread_impl.cpp"
#endif

namespace dbp {

thread::thread(): pimpl(NULL) {
	pimpl = new local::thread_impl();
};

thread::thread(const thread &src) {
	pimpl = new local::thread_impl();
	_handler = src._handler;
	pimpl->on_execute(_handler);
}

thread::~thread() {
	delete pimpl;
};

void thread::start() {
	pimpl->start();
};

void thread::stop() {
	pimpl->stop();
};

void thread::pause() {
	pimpl->pause();
};

void thread::resume() {
	pimpl->resume();
};

void thread::on_execute(thread_int::execute_handler handler) {
	_handler = handler;
	pimpl->on_execute(handler);
};

void thread::wait_for() {
	pimpl->wait_for();
};

} // namespace

