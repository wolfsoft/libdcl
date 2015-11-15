/*
 * event_impl.cpp
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

#include <pthread.h>

#include "dcl/event.h"

namespace dbp {
namespace local {

using namespace dbp;

class event_impl: public event_int {
public:
	event_impl(void *m): _m(static_cast<pthread_mutex_t*>(m)) {
		pthread_cond_init(&_event, NULL);
	}
	virtual ~event_impl() {
		pthread_cond_destroy(&_event);
	}
	virtual void wait() {
		pthread_cond_wait(&_event, _m);
	}
	virtual void raise() {
		pthread_cond_broadcast(&_event);
	}
private:
	pthread_cond_t _event;
	pthread_mutex_t *_m;
};

}} // namespace

