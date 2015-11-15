/*
 * process.cpp
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

#include "dcl/process.h"

#ifdef _WIN32
#include "win32/process_impl.cpp"
#else
#include "posix/process_impl.cpp"
#endif

namespace dbp {

process::process() {
	pimpl = new local::process_impl();
}

process::process(std::istream &in, std::ostream &out,
  std::ostream &err) {
	pimpl = new local::process_impl(in, out, err);
}

process::~process() {
	delete pimpl;
}

int process::execute(const std::string &path, const std::string &args) {
	pimpl->execute(path, args);
}

} // namespace
