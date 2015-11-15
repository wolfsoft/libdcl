/*
 * semaphore.cpp
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

#include "dcl/semaphore.h"

#ifdef _WIN32
#include "win32/semaphore_impl.cpp"
#else
#include "posix/semaphore_impl.cpp"
#endif

namespace dbp {

semaphore::semaphore(int initial_value): pimpl(NULL) {
	pimpl = new local::semaphore_impl(initial_value);
}

semaphore::~semaphore() {
	delete pimpl;
}

void semaphore::lock() {
	pimpl->lock();
}

bool semaphore::trylock() {
	pimpl->trylock();
}

void semaphore::unlock() {
	pimpl->unlock();
}

} //namespace

