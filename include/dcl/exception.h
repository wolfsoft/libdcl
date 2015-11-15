/*
 * exception.h
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

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

namespace dbp {

//!	Generic exception class.
/*!
	The generic exception class is used as parent class for all kinds of
	exceptions in the library.
*/
class exception: public std::exception {
public:
	//! Constructor
	exception(const std::string &msg = "") throw(): _msg(msg) { }
	//! Destructor
	virtual ~exception() throw() { }
	//! Error message
	/*!
		Returns a C-style character string describing the general
		cause of the current error.
	*/
	virtual const char* what() const throw() {
		return _msg.c_str();
	}
protected:
	const std::string& get_message() const throw() {
		return _msg;
	}
private:
	std::string _msg;
};

} // namespace

#endif /*_EXCEPTION_H_*/
