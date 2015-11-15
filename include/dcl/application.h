/*
 * application.h
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

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <string>
#include <map>

#include <dcl/delegate.h>
#include <dcl/cmdline_parameters.h>
#include <dcl/singleton.h>
#include <dcl/logger.h>

namespace dbp {

//! Base application interface class
/*!
	Each concrete application should implement this interface by overriding its
	pure virtual methods.
*/
class application_int {
public:
	//! Command line parameter handler
	typedef delegate1<cmdline_parameter&, bool> cmdline_parameter_handler;
	//! Execute command handler
	typedef delegate0<int> on_execute_handler;
	//! Exception handler
	typedef delegate1<const std::exception&, void> on_exception_handler;
	//! Destructor
	virtual ~application_int() { };
	//!	Set application name
	/*!
		The application name is used for displaying purposes. The
		internationalization (i18n) resources path (catalog) is
		also initialized from the application name.
		
		\param name the name of the application.
	*/
	virtual void set_name(const std::string &name) = 0;
	//!	Set application description
	/*!
		\param description a brief application description for displaying
		purposes.
	*/
	virtual void set_description(const std::string &description) = 0;
	//!	Register the application command line parameter
	/*!
		You can register one or more command line parameters supported by
		your application. When application starts, it checks of command
		line parameters passed for registered ones. If none is found, it
		reports about invalid command line parameter; if registered one is
		found, on_cmdline_parameter handler is called.

		See the 'examples/hello_world/console_application' directory for the
		sample code using this method.

		\param param the command line parameter to register.
		\param handler a command line parameter delegate.
	*/
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  cmdline_parameter_handler handler) = 0;
	//! Register the application main execute cycle handler
	/*!
		You need to register your own application execution handler with
		application logic.

		See the 'examples/hello_world/console_application' directory for the
		sample code.

		\param handler an execute command delegate.
	*/
	virtual void on_execute(on_execute_handler handler) = 0;
	//! Register the exception handler
	virtual void on_exception(on_exception_handler handler) = 0;
	//! Execute the application
	virtual int run(int argc, char *argv[]) = 0;
	//! Returns logger
	virtual dbp::logger& get_logger() = 0;
};

//!	Console application class
/*!
	The basic application class. If you want to create simple, generic
	application (console utility, for example), you need to use this
	class, for example:

	\include hello_world/console_application/main.cpp
*/
class application: public application_int,
  public singleton<application> {
	friend class singleton<application>;
public:
	virtual void set_name(const std::string &app_name);
	virtual void set_description(const std::string &description);
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  application_int::cmdline_parameter_handler handler);
	//! Register the application main execute cycle handler
	/*!
		You need to register your own application execution handler with
		application logic.

		See the 'examples/hello_world/console_application' directory for the
		sample code.

		\param handler an execute command delegate.
	*/
	virtual void on_execute(on_execute_handler handler);
	//! Register the exception handler
	virtual void on_exception(on_exception_handler handler);
	//! Execute the application
	virtual int run(int argc, char *argv[]);
	//! Get application name
	/*!
		Returns a value previously set by set_name() call.
	*/
	const std::string& get_name() const {
		return _app_name;
	};
	//! Get the application path
	/*!
		Returns a path to the the application executable.
	*/
	const std::string& get_path() const {
		return _app_path;
	};
	//! Get application description
	/*!
		Returns a value previously set by set_description() call.
	*/
	const std::string& get_description() const {
		return _app_description;
	};
	//! Returns the logger based on simple stdout/stderr
	virtual dbp::logger& get_logger()  {
		return logger;
	};
protected:
	application();
private:

	class console_logger: public dbp::logger {
	public:
		virtual void log(log_level::log_level level, const std::string &message);
	};

	std::string _app_path, _app_name, _app_description;
	cmdline_parameters _params;
	typedef std::map<char, cmdline_parameter_handler>
	  cmdline_parameter_handlers;
	cmdline_parameter_handlers _handlers;
	on_execute_handler execute_handler;
	on_exception_handler exception_handler;
	console_logger logger;
	bool init(int argc, char *argv[]);
	bool cmdline_help_handler(cmdline_parameter &param);
	void print_cmdline_parameters_help();
};

} // namespace

#undef IMPLEMENT_APP
#define IMPLEMENT_APP(app)													\
int main(int argc, char *argv[]) {											\
	return app.run(argc, argv); 	                                        \
};

#endif /*_APPLICATION_H_*/
