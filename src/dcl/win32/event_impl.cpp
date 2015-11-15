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

#include <windows.h>

#include "dcl/exception.h"
#include "dcl/i18n.h"
#include "dcl/event.h"

namespace dbp {
namespace local {

using namespace dbp;

class event_impl: public event_int {
public:
	event_impl(void *m): _m(static_cast<HANDLE*>(m)) {
		_event = CreateEvent(NULL, false, false, NULL);
	}
	virtual ~event_impl() {
		CloseHandle(_event);
	}
	virtual void wait() {
		SignalObjectAndWait(*_m, _event, INFINITE, false);
		WaitForSingleObject(*_m, INFINITE);
	}
	virtual void raise() {
		PulseEvent(_event);
	}
private:
	HANDLE *_m;
	HANDLE _event;
};

}} // namespace

