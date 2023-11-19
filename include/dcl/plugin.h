/*
 * plugin.h
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

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <string>

#include <dcl/exception.h>

namespace dbp {

//!	Disposable interface
/*!
	The disposable interface is a parent for all classes created by plugin
	class.
*/
class disposable {
public:
	//! Destructor
	virtual ~disposable() { }
};

//!	Plugin exception
/*!
	This exception class is used to signal about an error in plugin
	operations.
*/
class plugin_exception: public exception {
public:
	//! Constructor
	plugin_exception(const std::string &msg): exception(msg) { }
};

//! Plugin interface
/*!
	The plugin interface introduces pure virtual methods that are
	implemented by concrete plugin class.
*/
class plugin_int {
public:
	//! Destructor
	virtual ~plugin_int() { }
	//!	Load plugin
	/*!
		Loads the plugin from a given file which should be a dynamic library.

		\param file_name a name of the dynamic library file to load.
		\return a reference to a plugin object factory of the specified plugin.

		The plugin (dynamic library) should export 4 following C-functions:
		\code
void init(void*);
void* create_object(const char*);
void destroy_object(void*);
void finalize();
		\endcode

		First function should perform plugin initialization, if needed.

		Second function should create the specified class and return the pointer
		to the class instance created.
		
		Third function should destroy object created by
		create_object(const std::string&) and free up the memory.
		
		Fourth function should perform plugin finalization if needed.
	*/
	virtual void load(const std::string &file_name) = 0;
	//!	Initialize the plugin
	/*!
		This function initializes the plugin.

		\param data the pointer to the data passed by plugin caller app.
	*/
	virtual void init(void *data) = 0;
	//!	Create object
	/*!
		This function creates an object by its name. Each object created by this
		function should be properly destroyed by calling destroy_object to avoid
		memory leaks.

		\param object_name a name of the object to create.
		\return pointer to a object or NULL if this object is not supported.
	*/
	virtual disposable* create_object(const std::string &object_name) = 0;
	//! Destroy object
	/*!
		This function is used to destroy object created by
		create_object(const std::string&) function.
		
		\param object the object to destroy.
	*/
	virtual void destroy_object(disposable *object) = 0;
	//!	Finalize the plugin
	/*!
		This function uninitializes the plugin. Called automatically
		just before the plugin object destroyed.
	*/
	virtual void finalize() = 0;
};

//!	Plugin loader
/*!
	Plugin loader loads a dynamic library (.so, .dll) as a program plugin.
*/
class plugin: public plugin_int {
public:
	//! Constructor
	plugin();
	//! Destructor
	~plugin();
	virtual void init(void *data);
	virtual void load(const std::string &file_name);
	virtual disposable* create_object(const std::string &object_name);
	virtual void destroy_object(disposable *object);
protected:
	virtual void finalize();
private:
	plugin_int *_impl;
};

} // namespace


#endif /*_PLUGIN_H_*/
