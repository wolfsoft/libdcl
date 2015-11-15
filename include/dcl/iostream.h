/*
 * iostream.h
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

#ifndef _IOSTREAM_H_
#define _IOSTREAM_H_

#include <string>
#include <istream>
#include <ostream>

#include <dcl/exception.h>
#include <dcl/url.h>

namespace dbp {

//!	Input/Output stream exception class
/*!
	This exception class is used to signal about error in input/output
	operations.
*/
class iostream_exception: public exception {
public:
	//! Constructor
	iostream_exception(const std::string &msg): exception(msg) { }
};

//!	Input/Output stream class
/*!
	iostream class is useful to download data, provided by URL, into the
	standard input stream or upload the data from the standard output stream.
*/
class iostream: public std::istream, public std::ostream {
public:
	//! Constructor
	iostream(const url &url): _url(url) { }
private:
	const url &_url;
};

}

#endif /*_IOSTREAM_H_*/

