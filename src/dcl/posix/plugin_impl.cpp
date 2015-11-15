/*
 * plugin_impl.cpp
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

#include <dlfcn.h>

#include <dcl/strutils.h>
#include <dcl/plugin.h>

namespace dbp {
namespace local {
using namespace std;

class plugin_impl: public plugin_int {
public:
	plugin_impl(): handle(NULL) { };
	virtual ~plugin_impl();
	virtual void init(void *data);
	virtual void load(const std::string &file_name) throw(plugin_exception);
	virtual disposable* create_object(const std::string &object_name);
	virtual void destroy_object(disposable *object);
protected:
	virtual void finalize();
private:
	void* handle;
	std::string _filename;
	typedef void (*_init_func)(void*);
	typedef disposable* (*_create_object_func)(const char*);
	typedef void (*_destroy_object_func)(disposable*);
	typedef void (*_finalize_func)();
	_init_func _init;
	_create_object_func _create_object;
	_destroy_object_func _destroy_object;
	_finalize_func _finalize;
};

plugin_impl::~plugin_impl() {
	if (handle)
		dlclose(handle);
}

void plugin_impl::load(const std::string &file_name) throw(plugin_exception) {
	_filename = file_name;
	// loading a dynamic library
	if (handle)
		dlclose(handle);
	handle = dlopen(file_name.c_str(), RTLD_NOW);
	try {
		if (!handle)
			throw plugin_exception(dlerror());
		// search for the exported create_object function
		const char *error = NULL;
		_init = (_init_func) dlsym(handle, "init");
		if ((error = dlerror()) != NULL)
			throw plugin_exception(string(error));
		_create_object = (_create_object_func) dlsym(handle, "create_object");
		if ((error = dlerror()) != NULL)
			throw plugin_exception(string(error));
		_destroy_object = (_destroy_object_func)
		  dlsym(handle, "destroy_object");
		if ((error = dlerror()) != NULL)
			throw plugin_exception(string(error));
		_finalize = (_finalize_func) dlsym(handle, "finalize");
		if ((error = dlerror()) != NULL)
			throw plugin_exception(string(error));
	}
	catch (...) {
		if (handle)
			dlclose(handle);
		handle = NULL;
		// clear any existing error
		dlerror();
		throw;
	}
}

void plugin_impl::init(void *data) {
	_init(data);
}

disposable* plugin_impl::create_object(const std::string &object_name) {
	// call exported function to obtain an object instance
	disposable *rslt = _create_object(object_name.c_str());
	if (!rslt)
		throw plugin_exception(
		  (format(_("Plugin '{0}' can't create object '{1}'")) %
		    _filename % object_name).str());
	return rslt;
}

void plugin_impl::destroy_object(disposable *object) {
	_destroy_object(object);
}

void plugin_impl::finalize() {
	_finalize();
}

}} // namespace
