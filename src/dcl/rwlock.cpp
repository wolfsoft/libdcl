/*
 * rwlock.cpp
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2011 Dennis Prochko <wolfsoft@mail.ru>
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

#include "dcl/rwlock.h"

#ifdef _WIN32
#include "win32/rwlock_impl.cpp"
#else
#include "posix/rwlock_impl.cpp"
#endif

namespace dbp {

rwlock::rwlock(): pimpl(NULL) {
	pimpl = new local::rwlock_impl();
}

rwlock::~rwlock() {
	delete pimpl;
}

void rwlock::enter_read() {
	pimpl->enter_read();
}

bool rwlock::try_enter_read() {
	return pimpl->try_enter_read();
}

void rwlock::enter_write() {
	pimpl->enter_write();
}

bool rwlock::try_enter_write() {
	return pimpl->try_enter_write();
}

void rwlock::leave_read() {
	pimpl->leave_read();
}

void rwlock::leave_write() {
	pimpl->leave_write();
}

rwlock_guard_read::rwlock_guard_read(rwlock &m): _rwlock(m) {
	_rwlock.enter_read();
}

rwlock_guard_read::~rwlock_guard_read() {
	_rwlock.leave_read();
}

rwlock_guard_write::rwlock_guard_write(rwlock &m): _rwlock(m) {
	_rwlock.enter_write();
}

rwlock_guard_write::~rwlock_guard_write() {
	_rwlock.leave_write();
}

} //namespace

