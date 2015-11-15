/*
 * any.h
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

#ifndef _ANY_H_
#define _ANY_H_

#include <string>
#include <typeinfo>

#include <dcl/strutils.h>

namespace dbp {

//! Any container class
/*!
	The any container class is a holder for value of any type. It is useful
	to store objects of different types in the single STL container, for
	example. Moreover, you can cast one type to another with this class.
*/
class any {
	friend std::ostream& operator<<(std::ostream &str, const any &value);
public:
	//! Constructor
	any(): pimpl(NULL) { }
	//! Copy constructor
	any(const any &other): pimpl(NULL) {
		if (other.pimpl)
			pimpl = other.pimpl->clone();
	}
	//! Copy constructor for template value type
	template <class T>
	any(const T &other) {
		pimpl = new impl<T>(other);
	}
	//! Assignment operator
	const any& operator=(const any &other) {
		if (pimpl) delete pimpl;
		if (other.pimpl)
			pimpl = other.pimpl->clone();
		else
			pimpl = NULL;
		return *this;
	}
	//! Destructor
	virtual ~any() {
		if (pimpl) delete pimpl;
	}
	//! Clear value
	void clear() {
		if (pimpl) delete pimpl;
		pimpl = NULL;
	}
	//! Check for empty value
	/*!
		\returns true, if the value is not defined and false otherwise.
	*/
	bool empty() const {
		return (pimpl == NULL);
	}
	//! Determine the value type
	/*!
		\returns typeid(void) if the value is not defined, or the value type
		otherwise.
	*/
	const std::type_info& type() const {
		if (pimpl)
			return pimpl->type();
		else
			return typeid(void);
	}
	//! Convert value to a string
	/*!
		\returns the string representation of the value.
	*/
	std::string str() const {
		std::string tmp;
		if (pimpl)
			return pimpl->str();
		else
			return tmp;
	}
	//! Cast to another type
	/*!
		The cast() template method helps to cast value to the other type.
		If the native value type and the type requested are differs the
		safe conversion via string representation is occured.
		The first cast to the different type is a slow operation
		(3200 double/int casts per second @ one core of
		Athlon 64 X2 5200+), but future calls are ~300 times faster.
	*/
	template <typename T>
	const T cast() {
		if (!pimpl) {
			T tmp;
			return tmp;
		} else {
			if (pimpl->type() != typeid(T)) {
				impl_int *saved = pimpl;
				pimpl = new impl<T>(*saved);
				if (saved) delete saved;
			}
			return static_cast<impl<T>* >(pimpl)->cast();
		}
	}
	//! Equality operator
	bool operator==(const any &rhs) const {
		return (!pimpl && !rhs.pimpl) ||
		  ((pimpl && rhs.pimpl) && (*pimpl == *(rhs.pimpl)));
	}
private:
	class impl_int {
	public:
		virtual ~impl_int() { };
		virtual const std::type_info& type() const = 0;
		virtual impl_int* clone() const = 0;
		virtual bool operator==(const impl_int&) const = 0;
		virtual std::string str() const = 0;
	};
	template <class T>
	class impl: public impl_int {
	public:
		impl(const impl_int &value) {
			_value = from_string<T>(value.str());
		};
		impl(const T &value): _value(value) { };
		virtual const std::type_info& type() const {
			return typeid(_value);
		};
		virtual impl_int* clone() const {
			return new impl<T>(_value);
		};
		virtual bool operator==(const impl_int &other) const {
			return (_value == static_cast<impl>(other)._value);
		};
		virtual std::string str() const {
			return to_string<T>(_value);
		};
		const T cast() const {
			return _value;
		};
	private:
		T _value;
	};
	impl_int *pimpl;
};

//! Convert value to a string and put it into a stream
inline std::ostream& operator<<(std::ostream &str, const any &value) {
	str << value.str();
	return str;
};

} // namespace

#endif /*_ANY_H_*/

