/*
 * exception_system.h
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

#ifndef _EXCEPTION_SYSTEM_H_
#define _EXCEPTION_SYSTEM_H_

#include <string>

#include <windows.h>

#include <dcl/exception.h>
#include <dcl/strutils.h>

namespace dbp {

class exception_system: public dbp::exception {
public:
	exception_system(int c, const std::string &msg): dbp::exception(msg),
	  code(c) { }
	virtual const char* what() const noexcept {
		char *m = NULL;
		FormatMessage(
		  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		  FORMAT_MESSAGE_FROM_SYSTEM |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL, code, 0, (LPTSTR)&m, 0, NULL);
		if (m) {
			std::string rslt = get_message() + ". " + trim()(std::string(m));
			LocalFree(m);
			return rslt.c_str();
		}
		return NULL;
	}
private:
	int code;
};

}

#endif /*_EXCEPTION_SYSTEM_H_*/
