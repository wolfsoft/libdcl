/*
 * semaphore_impl.cpp
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

#include <windows.h>

#include "dcl/semaphore.h"

namespace dbp {
namespace local {

using namespace dbp;

#define MAX_SEM_COUNT 0x7FFFFFFF

class semaphore_impl: public semaphore_int {
public:
	semaphore_impl(int initial_value = 0) {
		_sem = CreateSemaphore(NULL, initial_value, MAX_SEM_COUNT, NULL);
	}
	virtual ~semaphore_impl() {
		CloseHandle(_sem);
	}
	virtual void lock() {
		WaitForSingleObject(_sem, INFINITE);
	}
	virtual bool trylock() {
		return WaitForSingleObject(_sem, 0) == WAIT_OBJECT_0;
	}
	virtual void unlock() {
		ReleaseSemaphore(_sem, 1, NULL);
	}
private:
	HANDLE _sem;
};

}} // namespace

