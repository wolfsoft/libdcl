/*
 * rwlock_impl.cpp
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

#include "dcl/dclbase.h"

namespace dbp {
namespace local {

using namespace dbp;

class rwlock_impl: public rwlock_int {
public:
	rwlock_impl() {
		pthread_rwlockattr_t attr;
		pthread_rwlockattr_init(&attr);
		pthread_rwlock_init(&_rwlock, &attr);
	}
	virtual ~rwlock_impl() {
		pthread_rwlock_destroy(&_rwlock);
	}
	virtual void enter_read() {
		switch (pthread_rwlock_rdlock(&_rwlock)) {
			case EDEADLK:
				throw rwlock_exception(_("the rwlock is already locked by the calling thread"));
				break;
			case EINVAL:
				throw rwlock_exception(_("the rwlock has not been properly initialized"));
				break;
		}
	}
	virtual bool try_enter_read() {
		switch (pthread_rwlock_tryrdlock(&_rwlock)) {
			case EBUSY:
				return true;
				break;
			case EINVAL:
				throw rwlock_exception(_("the rwlock has not been properly initialized"));
				break;
		}
		return false;
	}
	virtual void leave_read() {
		switch (pthread_rwlock_unlock(&_rwlock)) {
			case EPERM:
				throw rwlock_exception(_("the calling thread does not own the rwlock"));
				break;
			case EINVAL:
				throw rwlock_exception(_("the rwlock has not been properly initialized"));
				break;
		}
	}
	virtual void enter_write() {
		switch (pthread_rwlock_wrlock(&_rwlock)) {
			case EDEADLK:
				throw rwlock_exception(_("the rwlock is already locked by the calling thread"));
				break;
			case EINVAL:
				throw rwlock_exception(_("the rwlock has not been properly initialized"));
				break;
		}
	}
	virtual bool try_enter_write() {
		switch (pthread_rwlock_trywrlock(&_rwlock)) {
			case EBUSY:
				return true;
				break;
			case EINVAL:
				throw rwlock_exception(_("the rwlock has not been properly initialized"));
				break;
		}
		return false;
	}
	virtual void leave_write() {
		switch (pthread_rwlock_unlock(&_rwlock)) {
			case EPERM:
				throw rwlock_exception(_("the calling thread does not own the rwlock"));
				break;
			case EINVAL:
				throw rwlock_exception(_("the rwlock has not been properly initialized"));
				break;
		}
	}
private:
	pthread_rwlock_t _rwlock;
};

}} // namespace

