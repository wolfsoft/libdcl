/*
 * mimetype.cpp
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2008-2009 Dennis Prochko <wolfsoft@mail.ru>
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

#include <dcl/filefs.h>
#include <dcl/strutils.h>
#include <dcl/mimetype.h>

namespace dbp {

using namespace std;

mimetype::mimetype(): default_type("text/plain") {
	filefs fs;
	try {
		load_from_file(fs.get_system_config_dir() +
		  fs.get_path_delimiter() +
		  "mime.types");
	}
	catch (...) { }
	try {
		load_from_file(fs.get_home_dir() + fs.get_path_delimiter() +
		  ".mime.types");
	}
	catch (...) { }
}

void mimetype::load_from_file(const std::string &filename) {
	ifstream fs(filename.c_str(), ios::in);
	string line;
	while (getline(fs, line)) {
		line = trim()(line);
		// is it a comment?
		if ((line.empty()) ||
		    (line[0] == '#') ||
		    (line[0] == ';')) continue;
		// split line into a pair
		string::size_type pos;
		pos = line.find_first_of(" \t", 1);
		if (pos != string::npos) {
			string m_type = line.substr(0, pos);
			strings exts = tokenize()(trim()(line.substr(pos + 1)), " \t");
			for (strings::const_iterator i = exts.begin();
			  i != exts.end(); ++i) {
			  if (!i->empty())
				mime[*i] = m_type;
			}
		}
	}
}

void mimetype::register_type(const std::string &ext,
  const std::string &mime_type) {
	mime[ext] = mime_type;
}

const std::string& mimetype::operator()(const std::string& filename) const {
	// split filename into the name and extension
	string name, ext;
	tokenize()(filename, name, ext, true, ".");
	// find the MIME type in the database
	mime_types::const_iterator it = mime.find(ext);
	if (it != mime.end())
		return it->second;
	else
		return default_type;
}

}

