/*
 * plugin.cpp
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

#include <dcl/plugin.h>

#ifdef _WIN32
#include "win32/plugin_impl.cpp"
#else
#include "posix/plugin_impl.cpp"
#endif

namespace dbp {

using namespace std;

plugin::plugin() {
	_impl = new local::plugin_impl();
}

plugin::~plugin() {
	finalize();
	delete _impl;
}

void plugin::load(const std::string &filename) {
	_impl->load(filename);
}

void plugin::init(void *data) {
	_impl->init(data);
}

disposable* plugin::create_object(const std::string &object_name) {
	return _impl->create_object(object_name);
}

void plugin::destroy_object(disposable *object) {
	_impl->destroy_object(object);
}

void plugin::finalize() {
	_impl->finalize();
}

} // namespace
