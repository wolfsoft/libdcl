/*
 * process.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2010 Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <string>

#include <dcl/exception.h>

namespace dbp {

//! Process exception
/*!
	This exception class is raised on errors in process class.
*/
class process_exception: public exception {
public:
	process_exception(const std::string &msg): exception(msg) { }
};

//! Process interface
/*!
	This is an interface to the process class. Any concrete process class
	should subclass from this and implement pure virtual methods.
*/
class process_int {
public:
	virtual ~process_int() { };
	virtual int execute(const std::string &path, const std::string &args) = 0;
};

//! System process
/*!
	This class represents the system process.
*/
class process: public process_int {
public:
	//! Constructor
	process();
	//! Constructor
	process(std::istream &in, std::ostream &out, std::ostream &err);
	//! Destructor
	virtual ~process();
	virtual int execute(const std::string &path, const std::string &args);
private:
	process_int *pimpl;
};

} // namespace

#endif /*_PROCESS_H_*/
