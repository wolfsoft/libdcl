/*
 * filefs_impl.cpp
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

#include <unistd.h>
#include <errno.h>

#include <glob.h>

#include <dcl/filesystem.h>
#include <dcl/strutils.h>

namespace dbp {

namespace local {

char* _getcwd () {
	size_t size = 100;
	while (1) {
		char *buffer = new char[size];
		if (getcwd(buffer, size) == buffer)
			return buffer;
		delete[] buffer;
		if (errno != ERANGE)
			return 0;
		size *= 2;
	}
}

class filefs_impl: public filesystem {
public:
	filefs_impl(): _need_globfree(false) { }
	virtual ~filefs_impl() {
		if (_need_globfree)
			globfree(&g);
	}
	virtual std::string get_path_delimiter() const {
		return "/";
	}
	virtual std::string get_current_dir() const {
		char* cwd = _getcwd();
		std::string s;
		if (cwd)
			s = cwd;
		delete[] cwd;
		return s;
	}
	virtual void set_current_dir(const std::string &name) const {
		chdir(name.c_str());
	}
	virtual std::string get_temp_dir() const {
		return "/tmp";
	}
	virtual std::string get_home_dir() const {
		return "~";
	}
	virtual std::string get_locale_dir() const {
		return LOCALEDIR;
	}
	virtual std::string get_system_config_dir() const {
		return SYSCONFDIR;
	}
	virtual std::string get_data_dir() const {
		return DATAROOTDIR;
	}
	virtual bool find_first_file(const std::string &pattern,
	  std::string &filename) {
		cur = 0;
		if (_need_globfree) {
			globfree(&g);
			_need_globfree = false;
		}
		if (glob(pattern.c_str(), 0, NULL, &g) == 0) {
			_need_globfree = true;
			return find_next_file(filename);
		} else {
			return false;
		}
	}
	virtual bool find_next_file(std::string &filename) {
		if (cur < g.gl_pathc) {
			filename = g.gl_pathv[cur];
			cur++;
			return true;
		}
		else {
			filename = "";
			return false;
		}
	}
private:
	glob_t g;
	bool _need_globfree;
	size_t cur;
};

}} // namespace

