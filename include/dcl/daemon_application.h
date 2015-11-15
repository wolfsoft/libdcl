/*
 * daemon_application.h
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

#ifndef _DAEMON_APPLICATION_H_
#define _DAEMON_APPLICATION_H_

#include <string>

#include <dcl/application.h>

namespace dbp {

//! Daemon application interface
/*!
	Each concrete daemon application should implement this interface by
	implementing pure virtual methods.
*/
class daemon_application_int: public application_int {
public:
	//! Reload application configuration signal handler
	typedef delegate0<void> on_reload_configuration_handler;
	//! Pause signal handler
	typedef delegate0<void> on_pause_handler;
	//! Resume signal handler
	typedef delegate0<void> on_resume_handler;
	//! Terminate signal handler
	typedef delegate0<void> on_terminate_handler;
	//! Register "reload application configuration" signal handler
	/*!
		If your application implements of on-the-fly reconfiguration,
		you should register your own signal handler. This signal is raised
		when an user wants to reload daemon configuration file to apply
		new configuration parameters.
		
		\param handler the signal handler delegate
	*/
	virtual void on_reload_configuration(on_reload_configuration_handler handler) = 0;
	//! Register "pause" signal handler
	/*!
		The "pause" signal is raised when an user wants to pause the
		application. If your application should react to pause (by
		stopping calculations or dropping new connections), you should
		register your own signal handler.

		\param handler the signal handler delegate
	*/
	virtual void on_pause(on_pause_handler handler) = 0;
	//! Register "resume" signal handler
	/*!
		The "resume" signal is raised when an user wants to continue the
		running of application. If your application should react to resume (by
		starting calculations or accepting new connections), you should
		register your own signal handler.

		\param handler the signal handler delegate
	*/
	virtual void on_resume(on_resume_handler handler) = 0;
	//! Register "terminate" signal handler
	/*!
		The "terminate" signal is raised when an user wants to stop the
		application. If your application should react to terminating (flushing
		buffers, etc.), you should register your own signal handler.

		\param handler the signal handler delegate
	*/
	virtual void on_terminate(on_terminate_handler handler) = 0;
};

//! Daemon application class
/*!
	This class implements daemons (services in Windows terms). These kinds
	of applications are running transparently in the system.
*/
class daemon_application: public daemon_application_int,
  public singleton<daemon_application> {
	friend class singleton<daemon_application>;
public:
	virtual void set_name(const std::string &name) {
		pimpl->set_name(name);
	};
	virtual void set_description(const std::string &description) {
		pimpl->set_description(description);
	};
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  cmdline_parameter_handler handler) {
		pimpl->register_cmdline_parameter(param, handler);
	};
	virtual void on_execute(on_execute_handler handler) {
		pimpl->on_execute(handler);
	};
	virtual void on_exception(on_exception_handler handler) {
		pimpl->on_exception(handler);
	};
	virtual int run(int argc, char *argv[]) {
		return pimpl->run(argc, argv);
	};
	virtual void on_reload_configuration(on_reload_configuration_handler handler) {
		pimpl->on_reload_configuration(handler);
	};
	virtual void on_pause(on_pause_handler handler) {
		pimpl->on_pause(handler);
	};
	virtual void on_resume(on_resume_handler handler) {
		pimpl->on_resume(handler);
	};
	virtual void on_terminate(on_terminate_handler handler) {
		pimpl->on_terminate(handler);
	};
	//! Returns the logger based on system logging mechanism
	virtual dbp::logger& get_logger()  {
		return pimpl->get_logger();
	};
	
protected:
	daemon_application();
private:
	daemon_application_int *pimpl;
};

} // namespace

#endif /*_DAEMON_APPLICATION_H_*/
