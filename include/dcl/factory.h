/*
 * factory.h
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

#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <string>

#ifdef HAVE_CXX11
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <dcl/exception.h>

namespace dbp {

namespace internal {

template <class ParentT, class T>
ParentT* create_instance() {
	return new T();
};

} // namespace

//!	Object factory exception
/*!
	The exception class is used to signal about errors in the factory class.
*/
class factory_exception: public dbp::exception {
public:
	//! Constructor
	factory_exception(const std::string &msg): exception(msg) { }
};

class factory_non_reg_exception: public factory_exception {
public:
	factory_non_reg_exception(const std::string &msg):
	  factory_exception(msg) { }
	virtual const char* what() const noexcept;
};

class factory_already_reg_exception: public factory_exception {
public:
	factory_already_reg_exception(const std::string &msg):
	  factory_exception(msg) { }
	virtual const char* what() const noexcept;
};

class factory_unreg_exception: public factory_exception {
public:
	factory_unreg_exception(const std::string &msg):
	  factory_exception(msg) { }
	virtual const char* what() const noexcept;
};

//!	Object factory class template
/*!
	This class template is intented to provide object factory design pattern.
	It is used for creating objects dynamically by class name.
*/
template <class T>
class factory {
public:
	//! Create object
	/*!
		Creates the previously registered class instance. You should manually
		destroy the created object to prevent memory leaks.
		
		\param name the class name.
		\return the pointer to class instance.
	*/
	T* create(const std::string &name) {
		if (!is_registered(name))
			throw factory_non_reg_exception(name);
		return _creator[name]();
	};
	//! Register class
	/*!
		Registers the class RealT with given name.
		
		\param name the name of the class
	*/
	template <class RealT>
	void register_class(const std::string &name) {
		if (is_registered(name))
			throw factory_already_reg_exception(name);
		_creator[name] = &internal::create_instance<T, RealT>;
	};
	//! Unregister class
	/*!
		Deletes class registration.
		
		\param name the registered class name
	*/
	void unregister_class(const std::string &name) {
		if (!_creator.erase(name))
			throw factory_unreg_exception(name);
	};
	//! Check class registration state
	/*!
		Returns true if the class with the given name is registered and
		false if not.
		
		\return the class registration state
	*/
	bool is_registered(const std::string &name) {
		return _creator.find(name) != _creator.end();
	};
private:
	typedef T *(*create_instance_func)();
#ifdef HAVE_CXX11
	std::unordered_map<std::string, create_instance_func> _creator;
#else
	std::tr1::unordered_map<std::string, create_instance_func> _creator;
#endif
};

} // namespace

#endif /*_FACTORY_H_*/

