/*
 * mutex_impl.cpp
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

#include <windows.h>

#include "dcl/mutex.h"

namespace dbp {
namespace local {

using namespace std;

class mutex_impl: public mutex_int {
public:
	mutex_impl() {
		_mutex = CreateMutex(NULL, false, NULL);
	};
	virtual ~mutex_impl() {
		CloseHandle(_mutex);
	};
	virtual void enter() {
		WaitForSingleObject(_mutex, INFINITE);
	};
	virtual bool try_enter() {
		return WaitForSingleObject(_mutex, 0) == WAIT_OBJECT_0;
	};
	virtual void leave() {
		ReleaseMutex(_mutex);
	};
protected:
	virtual void* handle() {
		return &_mutex;
	}
private:
	HANDLE _mutex;
};

}} //namespace

