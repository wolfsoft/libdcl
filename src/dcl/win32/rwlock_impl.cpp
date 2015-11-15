/*
 * rwlock_impl.cpp
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

#include <windows.h>

#include "dcl/rwlock.h"

namespace dbp {
namespace local {

using namespace dbp;

#define MAX_READERS 64

class rwlock_impl: public rwlock_int {
public:
	rwlock_impl() {
		_sem = CreateSemaphore(NULL, MAX_READERS, MAX_READERS, NULL);
		_mutex = CreateMutex(NULL, false, NULL);
	}
	virtual ~rwlock_impl() {
		CloseHandle(_mutex);
		CloseHandle(_sem);
	}
	virtual void enter_read() {
		WaitForSingleObject(_sem, INFINITE);
	}
	virtual bool try_enter_read() {
		return WaitForSingleObject(_sem, 0) == WAIT_OBJECT_0;
	}
	virtual void leave_read() {
		ReleaseSemaphore(_sem, 1, NULL);
	}
	virtual void enter_write() {
		WaitForSingleObject(_mutex, INFINITE);
		for (int i = 0; i < MAX_READERS; ++i)
			WaitForSingleObject(_sem, INFINITE);
		ReleaseMutex(_mutex);
	}
	virtual bool try_enter_write() {
		if (WaitForSingleObject(_mutex, 0) != WAIT_OBJECT_0)
			return false;
		return WaitForSingleObject(_sem, 0) == WAIT_OBJECT_0;
	}
	virtual void leave_write() {
		ReleaseSemaphore(_sem, MAX_READERS, NULL);
	}
private:
	HANDLE _sem;
	HANDLE _mutex;
};

}} // namespace

