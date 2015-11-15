/*
 * app_config.cpp
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

#include <fstream>

#include "dcl/app_config.h"
#include "dcl/strutils.h"
#include "dcl/filefs.h"

namespace dbp {

using namespace std;

void app_config::clear() {
	// clear up existing options
	options.clear();
}

void app_config::load_from_file(const string &name) {
	clear();
	do_load(name);
}

void app_config::do_load(const string &name) {
	// open configuration file for reading
	ifstream config(name.c_str(), ios::in);
	if (!config) {
		throw app_config_exception(_("configuration file not found; "
		  "default values will be used"));
	}
	// read file line by line
	string line, current_section;
	string::size_type pos;
	while (getline(config, line)) {
		line = trim()(line);
		// is it a comment?
		if ((line.empty()) ||
		    (line[0] == '#') ||
		    (line[0] == ';')) continue;
		// is it a section?
		if (line[0] == '[') {
			// yes, assign this section to current
			current_section = trim()(line.substr(1, line.find(']') - 1));
		} else {
			// no, it's a parameter; split string into pairs
			pos = line.find_first_of('=', 1);
			if (pos != string::npos) {
				// is it a @include parameter?
				if (trim()(line.substr(0, pos)) == "@include") {
					// load all files (by wildcard) specified in @include
					filefs fs;
					string path, f_name;
					string tmp = fs.get_current_dir();
					tokenize()(name, path, f_name);
					if (!path.empty())
						fs.set_current_dir(path);
					for (filefs::const_iterator i =
					  fs.find(trim()(line.substr(pos + 1)));
					  i != fs.end(); ++i) {
						do_load(*i);
					}	
					fs.set_current_dir(tmp);
				}
				else
					options[current_section][trim()(line.substr(0, pos))] = 
					  trim()(line.substr(pos + 1));
			}
		}
	}
}

template<>
const string& app_config::value<const string&>(const string &section,
  const string &name, const string &def_value) const {
	sections::const_iterator i = options.find(section);
	// if no section found, return default value
	if (i == options.end())
		return def_value;
	// if no key found, return default value
	strindex::const_iterator j = i->second.find(name);
	if (j == i->second.end()) {
		return def_value;
	}
	// return key's value
	return j->second;
}

template<>
string app_config::value<string>(const string &section,
  const string &name, string def_value) const {
	return value<const string&>(section, name, def_value);
}

template<>
const char* app_config::value<const char*>(const std::string &section,
  const std::string &name, const char *def_value) const {
	return value<const string&>(section, name,
	  string(def_value)).c_str();
}

template<>
double app_config::value<double>(const string &section,
  const string &name, double def_value) const {
	return from_string<double>(value(section, name,
	  to_string<double>(def_value)));
}

template<>
int app_config::value<int>(const string &section,
  const string &name, int def_value) const {
	return from_string<int>(value(section, name, to_string<int>(def_value)));
}

template<>
bool app_config::value<bool>(const string &section,
  const string &name, bool def_value) const {
	string s = value(section, name, to_string<bool>(def_value));
	return (compare()(s, "YES") ||
	  compare()(s, "ON") ||
	  compare()(s, "TRUE") ||
	  compare()(s, "1"));
}

const app_config::strindex& app_config::section(
  const string &section_name) const {
	sections::const_iterator i = options.find(section_name);
	if (i == options.end())
		return _empty;
	else
		return i->second;
}

app_config::sections::const_iterator app_config::get_sections() {
	return options.begin();
}

app_config::sections::const_iterator app_config::end() {
	return options.end();
}

} /*namespace*/
