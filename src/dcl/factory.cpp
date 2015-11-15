/*
 * factory.cpp
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

#include <dcl/factory.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

const char* factory_non_reg_exception::what() const throw() {
	return (format(_("Can't create class {0}: the class with this name "
	  "is not registered")) % get_message()).str().c_str();
}

const char* factory_already_reg_exception::what() const throw() {
	return (format(_("Can't register class {0}: the class with this name "
	  "is already registered")) % get_message()).str().c_str();
}

const char* factory_unreg_exception::what() const throw() {
	return (format(_("Can't unregister class {0}: the class is not "
	  "registered")) % get_message()).str().c_str();
}

} // namespace

