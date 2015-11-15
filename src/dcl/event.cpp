/*
 * event.cpp
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

#include "dcl/event.h"

#ifdef _WIN32
#include "win32/event_impl.cpp"
#else
#include "posix/event_impl.cpp"
#endif

namespace dbp {

event::event(mutex &m): pimpl(NULL), _m(m) {
	pimpl = new local::event_impl(_m.pimpl->handle());
}

event::~event() {
	delete pimpl;
}

void event::wait() {
	pimpl->wait();
}

void event::raise() {
	pimpl->raise();
}

} //namespace
