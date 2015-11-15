/*
 * uuid.h
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

#ifndef _UUID_H_
#define _UUID_H_

#include <string>

namespace dbp {

//!	UUID class
/*!
	This class represents a Universally Unique IDentifier (UUID) number,
	as defined by RFC 4122.
*/
class uuid {
	//!	Set content from a stream
	friend std::istream& operator>>(std::istream&, uuid&);
	//!	Get a stream from content
	friend std::ostream& operator<<(std::ostream&, const uuid&);
public:
	//! Constructor
	uuid();
	//! Constructor
	uuid(const std::string&);
	//! Constructor
	uuid(const char*);
	//! Copy constructor
	uuid(const uuid&);
	//! Assignment operator
	uuid& operator=(const uuid&);
	//! Comparison operator
	bool operator==(const uuid&) const;
	//! Comparison operator
	bool operator<(const uuid&) const;
	//! Comparison operator
	bool operator>(const uuid &rhs) const {
		return (rhs < *this);
	}
	//! Comparison operator
	bool operator<=(const uuid &rhs) const {
		return !(rhs < *this);
	}
	//! Comparison operator
	bool operator>=(const uuid& rhs) const {
		return !(*this < rhs);
	}
	//! Comparison operator
	bool operator!=(const uuid &rhs) const {
		return !(*this == rhs);
	}
	//! Comparison operator (check for empty value)
	bool operator!() const {
		return empty();
	}
	//! Safe Bool Idiom
	typedef bool (uuid::*bool_type)() const;
	//! Comparison operator (check for empty value)
	operator bool_type() const {
		return empty() ? NULL : &uuid::empty;
	}
	//! Generate new uuid value
	void generate();
	//! Clear (set to empty value)
	void clear();
	//! Check for empty value
	bool empty() const;
	//! Convert to string representation
	std::string str() const;
private:
	static const int size = 16;
	unsigned char _data[size];
	void assign(const std::string&);
};

}

#endif /*_UUID_H_*/

