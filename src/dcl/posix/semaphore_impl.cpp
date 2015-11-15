/*
 * semaphore_impl.cpp
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

#include <semaphore.h>

#include "dcl/semaphore.h"

namespace dbp {
namespace local {

using namespace dbp;

class semaphore_impl: public semaphore_int {
public:
	semaphore_impl(int initial_value = 0) {
		sem_init(&_sem, 0, initial_value);
	}
	virtual ~semaphore_impl() {
		sem_destroy(&_sem);
	}
	virtual void lock() {
		while (sem_wait(&_sem) != 0) { }
	}
	virtual bool trylock() {
		return sem_trywait(&_sem) == 0;
	}
	virtual void unlock() {
		sem_post(&_sem);
	}
private:
	sem_t _sem;
};

}} // namespace

