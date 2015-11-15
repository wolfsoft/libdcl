/*
 * gui_application.h
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

#ifndef _GUI_APPLICATION_H_
#define _GUI_APPLICATION_H_

#include <dcl/application.h>
#include <dcl/logger.h>
#include <dcl/plugin.h>
#include <dcl/widget_factory.h>

namespace dbp {

//! GUI (Graphical User Interface) application class
/*!
	This is a platform-independed GUI application class. It is needed to
	create applications with graphical interface, for example:

	\include hello_world/gui_application/main.cpp
*/
class gui_application: public application_int,
  public singleton<gui_application> {
	friend class singleton<gui_application>;
public:
	virtual ~gui_application();
	virtual void set_name(const std::string &app_name) {
		pimpl->set_name(app_name);
	};
	virtual void set_description(const std::string &description) {
		pimpl->set_description(description);
	};
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  application_int::cmdline_parameter_handler handler) {
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
	//! Returns the logger based on simple stdout/stderr
	virtual dbp::logger& get_logger()  {
		return pimpl->get_logger();
	};
	//! Reference to the widget factory
	/*!
		You can obtain a reference to the GUI widget factory class to create
		any widgets supported by the DCL.

		\return the reference to the widget factory.
	*/
	widget_factory& factory() {
		return *_widget_factory;
	};
protected:
	gui_application();
private:
	plugin _plugin;
	widget_factory *_widget_factory;
	widget_factory::gui_application_ptr pimpl;
};

} // namespace

#endif /*_GUI_APPLICATION_H_*/

