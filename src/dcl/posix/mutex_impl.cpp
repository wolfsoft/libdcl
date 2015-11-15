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

#include <errno.h>
#include <pthread.h>

#include "dcl/exception.h"
#include "dcl/i18n.h"
#include "dcl/mutex.h"

namespace dbp {
namespace local {

using namespace dbp;

class mutex_impl: public mutex_int {
public:
	mutex_impl() {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutex_init(&_mutex, &attr);
	}
	virtual ~mutex_impl() {
		pthread_mutex_destroy(&_mutex);
	}
	virtual void enter() {
		switch (pthread_mutex_lock(&_mutex)) {
			case EDEADLK:
				throw mutex_exception(_("the mutex is already locked by the calling thread"));
				break;
			case EINVAL:
				throw mutex_exception(_("the mutex has not been properly initialized"));
				break;
		}
	}
	virtual bool try_enter() {
		switch (pthread_mutex_trylock(&_mutex)) {
			case EBUSY:
				return true;
				break;
			case EINVAL:
				throw mutex_exception(_("the mutex has not been properly initialized"));
				break;
		}
		return false;
	}
	virtual void leave() {
		switch (pthread_mutex_unlock(&_mutex)) {
			case EPERM:
				throw mutex_exception(_("the calling thread does not own the mutex"));
				break;
			case EINVAL:
				throw mutex_exception(_("the mutex has not been properly initialized"));
				break;
		}
	}
protected:
	virtual void* handle() {
		return &_mutex;
	}
private:
	pthread_mutex_t _mutex;
};

}} // namespace

