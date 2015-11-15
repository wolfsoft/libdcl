/*
 * gtk_application.h
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

#ifndef _GTK_APPLICATION_H_
#define _GTK_APPLICATION_H_

#include <dcl/gui_application.h>

namespace dbp {

class gtk_application: public application_int {
public:
	gtk_application();
	virtual void set_name(const std::string &name) {
		app.set_name(name);
	};
	virtual void set_description(const std::string &description) {
		app.set_description(description);
	};
	virtual void register_cmdline_parameter(const cmdline_parameter &param,
	  cmdline_parameter_handler handler) {
		app.register_cmdline_parameter(param, handler);
	};
	virtual void on_execute(on_execute_handler handler) {
		app.on_execute(handler);
	};
	virtual void on_exception(on_exception_handler handler) {
		app.on_exception(handler);
	};
	virtual int run(int argc, char *argv[]);
	//! Returns the logger based on simple stdout/stderr
	virtual dbp::logger& get_logger()  {
		return logger;
	};
private:
	class gtk_logger: public dbp::logger {
	public:
		virtual void log(log_level::log_level level, const std::string &message);
	};

	application &app;
	gtk_logger logger;

	void exception_handler(const std::exception &e);
	int execute_handler();
};

} // namespace

#endif /*_GTK_APPLICATION_H_*/
