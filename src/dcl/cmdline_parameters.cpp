/*
 * cmdline_parameters.cpp
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

#include <dcl/strutils.h>
#include <dcl/application.h>
#include <dcl/cmdline_parameters.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

namespace dbp {

using namespace std;


bool cmdline_parameter::operator==(const parameter_type p_type) const {
	return (type == p_type);
}

bool cmdline_parameter::operator==(const string &name) const {
	return (short_name == name[0]) || (long_name == name);
}

void cmdline_parameters::add(const cmdline_parameter &param) {
	_params.push_back(param);
}

bool cmdline_parameters::parse(int argc, char* argv[],
  cmdline_parameter &param) {
	// Check if we are returning filenames
	if (_pos > 0) {
		if (_pos < argc) {
			param.short_name = 0;
			param.value = argv[_pos];
			param.type = cmdline_parameter::FILENAME;
			_pos++;
			return true;
		} else {
			return false;
		}
	}
	// Construct short and long options for getopt()
	string optstring(":");
	#ifdef HAVE_GETOPT_H
		int c = 0;
		option *long_options = new option[_params.size()];
	#endif
	// Fill internal structure by valid commmand line parameters
	std::vector<cmdline_parameter>::const_iterator i;
	for (i = _params.begin(); i != _params.end(); ++i) {
		if ((i->type == cmdline_parameter::OPTION) ||
		  (i->type == cmdline_parameter::OPTION_WITH_VALUE)) {
			optstring += i->short_name;
			if (i->type == cmdline_parameter::OPTION_WITH_VALUE) {
				optstring += ":";
				#ifdef HAVE_GETOPT_H
					long_options[c].has_arg = 1;
				#endif
			}
			#ifdef HAVE_GETOPT_H
				long_options[c].flag = 0;
				long_options[c].name = const_cast<char*>(i->long_name.c_str());
				long_options[c].val = i->short_name;
				c++;
			#endif
		}	
	}
	// Parse command line options
	opterr = 0;
	#ifdef HAVE_GETOPT_H
		int r = getopt_long(argc, argv, optstring.c_str(),
		  long_options, &optind);
		delete[] long_options;
	#else
		int r = getopt(argc, argv, optstring.c_str());
	#endif
	switch (r) {
		case -1:
			if (optind < argc) {
				param.short_name = 0;
				param.value = argv[optind];
				param.type = cmdline_parameter::FILENAME;
				_pos = optind + 1;
				return true;
			} else
				return false;
			break;
		case '?':
			if (optopt)
				throw cmdline_parameters_exception(
				  (format(_("invalid option '{0}'")) % char(optopt)).str());
			else
				throw cmdline_parameters_exception(_("option unrecognized"));
			break;
		case ':':
			throw cmdline_parameters_exception(
			  (format(_("option '{0}' requires an argument")) %
			  char(optopt)).str());
			break;
		default:
			param.short_name = r;
			param.value = optarg ? optarg : "";
			return true;
	}
}

cmdline_parameters::params::const_iterator cmdline_parameters::begin() const {
	return _params.begin();
}

cmdline_parameters::params::const_iterator cmdline_parameters::end() const {
	return _params.end();
}

} // namespace
