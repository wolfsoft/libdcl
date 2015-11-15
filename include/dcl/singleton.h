/*
 * singleton.h
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

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <memory>

#include <dcl/mutex.h>
#include <dcl/noncopyable.h>

namespace dbp {

//!	Singleton class template
/*!
	This class template is intended to provide singleton pattern. It is
	useful for development of classes that should be exist in one
	instance only.
	
	Usage:
	\code
class test: public singleton<test> {
	friend class singleton<test>;
public:
	void foo();
	...
};
...
test &t = test::instance();
t.foo();
	\endcode
*/
template <class T>
class singleton: private noncopyable {
public:
	//! Obtain a singleton object reference
	/*!
		If the object was not instanceated, creates the instance. Returns
		the instance reference.
		
		\return the object instance reference
	*/
	static T& instance() {
		if (!_instance.get()) {
			mutex_guard m(_cs);
			if (!_instance.get())
				_instance.reset(new T());
		}
		return *_instance;
	};
	template <class C>
	static T& instance(C arg) {
		if (!_instance.get()) {
			mutex_guard m(_cs);
			if (!_instance.get())
				_instance.reset(new T(arg));
		}
		return *_instance;
	};
protected:
	singleton() { };
private:
#ifdef HAVE_CXX11
	static std::unique_ptr<T> _instance;
#else
	static std::auto_ptr<T> _instance;
#endif
	static mutex _cs;
};

template <class T>
#ifdef HAVE_CXX11
std::unique_ptr<T> singleton<T>::_instance;
#else
std::auto_ptr<T> singleton<T>::_instance;
#endif

template <class T>
mutex singleton<T>::_cs;

} //namespace

#endif /*_SINGLETON_H_*/
