/*
 * filefs.cpp
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

#include <dcl/filefs.h>

#ifdef _WIN32
#include "win32/filefs_impl.cpp"
#else
#include "posix/filefs_impl.cpp"
#endif

namespace dbp {

filefs::filefs() {
	pimpl = new local::filefs_impl();
}

filefs::~filefs() {
	delete pimpl;
}

std::string filefs::get_path_delimiter() const {
	return pimpl->get_path_delimiter();
}

std::string filefs::get_current_dir() const {
	return pimpl->get_current_dir();
}

void filefs::set_current_dir(const std::string &name) const {
	pimpl->set_current_dir(name);
}

std::string filefs::get_temp_dir() const {
	return pimpl->get_temp_dir();
}

std::string filefs::get_home_dir() const {
	return pimpl->get_home_dir();
}

std::string filefs::get_locale_dir() const {
	return pimpl->get_locale_dir();
}

std::string filefs::get_system_config_dir() const {
	return pimpl->get_system_config_dir();
}

std::string filefs::get_data_dir() const {
	return pimpl->get_data_dir();
}

bool filefs::find_first_file(const std::string &pattern,
  std::string &filename) {
	return pimpl->find_first_file(pattern, filename);
}

bool filefs::find_next_file(std::string &filename) {
	return pimpl->find_next_file(filename);
}

} // namespace

