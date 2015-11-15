/*
 * application.cpp
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

#include <iostream>
#include <iomanip>
#include <algorithm>

#include <dcl/application.h>
#include <dcl/datetime.h>
#include <dcl/strutils.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace dbp {

using namespace std;

application::application() {
	i18n::register_catalog(PACKAGE);
	register_cmdline_parameter(cmdline_parameter('h', "help",
	  _("show this help page")),
	  create_delegate(this, &application::cmdline_help_handler));
}

void application::set_name(const std::string &app_name) {
	_app_name = app_name;
	i18n::register_catalog(_app_name.c_str());
}

void application::set_description(const std::string &description) {
	_app_description = description;
}

void application::register_cmdline_parameter(const cmdline_parameter &param,
  cmdline_parameter_handler handler) {
	_params.add(param);
	_handlers[param.short_name] = handler;
}

void application::on_execute(on_execute_handler handler) {
	execute_handler = handler;
}

void application::on_exception(on_exception_handler handler) {
	exception_handler = handler;
}

int application::run(int argc, char *argv[]) {
	int rslt = -1;
	try {
		if (init(argc, argv) && execute_handler)
			rslt = execute_handler();
	}
	catch (std::exception &e) {
		if (exception_handler)
			exception_handler(e);
		else
			cout << format(_("Fatal error: {0}")) % e.what() << endl;
	}
	return rslt;
}

bool application::init(int argc, char *argv[]) {
	_app_path = argv[0];
	if (_app_name.empty()) {
		string tmp, def_name;
		tokenize()(_app_path, tmp, def_name);
		_app_name = def_name;
	}
	// parse command line arguments
	cmdline_parameter p;
	try {
		while (_params.parse(argc, argv, p)) {
			if (_handlers.find(p.short_name) != _handlers.end()) {
				if (!_handlers[p.short_name](p))
					return false;
			}
		}
	}
	catch (cmdline_parameters_exception &e) {
		// print syntax error message
		cerr << _app_name << ": " << _("syntax error") << " - " <<
		  e.what() << endl << endl;
		print_cmdline_parameters_help();
		return false;
	}
	return true;
}

bool application::cmdline_help_handler(cmdline_parameter &) {
	if (!_app_description.empty())
		cout << _app_name << ": " <<
		  _app_description << "." << endl << endl;
	print_cmdline_parameters_help();
	return false;
}

void application::print_cmdline_parameters_help() {
  // extract filename from path
	string path, filename;
	tokenize()(_app_path, path, filename);
	// print program usage
	cout << _("Usage: ") << filename << " ";
	// find option parameters, if such ones are registered
	if (find(_params.begin(), _params.end(),
	  cmdline_parameter::OPTION) != _params.end() ||
	  find(_params.begin(), _params.end(),
	  cmdline_parameter::OPTION_WITH_VALUE) != _params.end()) {
		cout << _("[options...] ");
	}
	// find command parameters, if such ones are registered
	cmdline_parameters::params::const_iterator i = find(_params.begin(),
	  _params.end(), cmdline_parameter::COMMAND);
	while (i != _params.end()) {
		cout << i->long_name << " ";
		++i;
	}
	// find filename parameters, if such ones are registered
	i = find(_params.begin(), _params.end(), cmdline_parameter::FILENAME);
	if (i != _params.end()) {
		if (i->description.empty())
			cout << _("file(s)...");
		else
			cout << i->description;
	}
	// print available options, if any
	if ((find(_params.begin(), _params.end(),
	  cmdline_parameter::OPTION) != _params.end()) ||
	  (find(_params.begin(), _params.end(),
	  cmdline_parameter::OPTION_WITH_VALUE) != _params.end())) {
		cout << endl << endl << _("Where available options are:") << endl;
		cmdline_parameters::params::const_iterator i;
		// calculate maximum parameter name length
		unsigned int max_length = 0;
		for (i = _params.begin(); i != _params.end(); ++i) {
			if ((i->type == cmdline_parameter::OPTION) ||
			  (i->type == cmdline_parameter::OPTION_WITH_VALUE)) {
				string s("-");
				s += i->short_name;
				if (!i->value_name.empty()) {
					s += " " + i->value_name;
				}
#ifdef HAVE_GETOPT_H
				if (!i->long_name.empty()) {
					s += ", --" + i->long_name;
				}
#endif
				if (!i->value_name.empty()) {
					s += "=" + i->value_name;
				}
				unsigned int i = char_length()(s);
				if (max_length < i) {
					max_length = i;
				}
			}
		}
		// print the list of available options
		for (i = _params.begin(); i != _params.end(); ++i) {
			if ((i->type == cmdline_parameter::OPTION) ||
			  (i->type == cmdline_parameter::OPTION_WITH_VALUE)) {
				string s("-");
				s += i->short_name;
				if (!i->value_name.empty()) {
					s += " " + i->value_name;
				}
#ifdef HAVE_GETOPT_H
				if (!i->long_name.empty()) {
					s += ", --" + i->long_name;
				}
#endif
				if (!i->value_name.empty()) {
					s += "=" + i->value_name;
				}
				if (!i->description.empty()) {
					cout << s << setw(max_length + 1 - char_length()(s)) <<
					  " " << setw(0) << " " << i->description;
				}
				else
					cout << s;
				cout << endl;
			}
		}
	}
}

void application::console_logger::log(log_level::log_level level, const std::string &message) {
	cout << "[" << datetime().now().str() << "] " << message << endl << flush;
}

} // namespace
