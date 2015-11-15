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

#include <windows.h>
#include <shlobj.h>

#include <dcl/filesystem.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

namespace local {

class filefs_impl: public filesystem {
public:
    filefs_impl(): h(0) { };
	virtual ~filefs_impl() {
		FindClose(h);
	}
	virtual string get_path_delimiter() const {
		return "\\";
	}
	virtual string get_current_dir() const {
		char dir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, dir);
		return string(dir);
	}
	virtual void set_current_dir(const std::string &name) const {
		SetCurrentDirectory(name.c_str());
	}
	virtual string get_temp_dir() const {
		TCHAR path[MAX_PATH];
		GetTempPath(sizeof(path), path);
		return string(path);
	}
	virtual string get_home_dir() const {
		TCHAR path[MAX_PATH];
		SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path);
		return string(path);
	}
	virtual string get_locale_dir() const {
		return string(get_root_dir() + string("\\share\\locale"));
	}
	virtual string get_system_config_dir() const {
		return string(get_root_dir() + string("\\etc"));
	}
	virtual string get_data_dir() const {
		return string(get_root_dir() + string("\\share"));
	}
	virtual bool find_first_file(const string &pattern,
	  string &filename) {
		string f_name;
		tokenize()(pattern, find_data_path, f_name);
		if (!find_data_path.empty())
		  find_data_path += get_path_delimiter();
		h = FindFirstFileA(pattern.c_str(), &find_data);
		if (h != INVALID_HANDLE_VALUE) {
			filename = find_data_path + find_data.cFileName;
			return true;
		}
		return false;
	}
	virtual bool find_next_file(string &filename) {
		filename = find_data_path + find_data.cFileName;
		return FindNextFileA(h, &find_data);
	}
private:
	HANDLE h;
	string find_data_path;
	WIN32_FIND_DATA find_data;
	string get_root_dir() const {
		TCHAR path[MAX_PATH];
		// obtain executable path
		GetModuleFileName(NULL, path, sizeof(path));
		// strip file name from dir name
		string dir, fname;
		tokenize()(to_string<const char*>(path), dir, fname);
		// if the last name is 'bin', strip it too
		string s;
		tokenize()(dir, s, fname);
		if (fname == string("bin"))
			return s;
		else
			return string(dir);
	}
};

}} // namespace

