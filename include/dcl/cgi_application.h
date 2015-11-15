/*
 * cgi_application.h
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

#ifndef _CGI_APPLICATION_H_
#define _CGI_APPLICATION_H_

#include <dcl/logger.h>
#include <dcl/singleton.h>
#include <dcl/web_application.h>

namespace dbp {

//!	CGI application class
/*!
	This class implements the web application on the CGI protocol.
	The cgi_application supports CGI (Common Gateway Interface) version 1.1,
	as described at RFC 3875.
	
	This is a most common web server extension format supported by all
	known extensible web servers. However, this protocol has many
	disadvantages including speed, memory usage, no pooling and
	limited session data.

	CGI application example:
	\include hello_world/cgi_application/main.cpp
*/
class cgi_application: public web_application_int,
  public singleton<cgi_application> {
	friend class singleton<cgi_application>;
public:
	virtual void on_handle_request(on_request_handler handler) {
		request_handler = handler;
	};
	virtual void on_exception(on_exception_handler handler) {
		exception_handler = handler;
	};
	//! Execute the application
	int run(int argc, char *argv[], char **env);
	//! Returns the logger based on simple stdout/stderr
	virtual dbp::logger& get_logger() {
		return logger;
	};
protected:
	http_request get_request(char **env);
	virtual http_request get_request();
	virtual void send_response(const http_response &response);
private:
	class cgi_logger: public dbp::logger {
	public:
		virtual void log(log_level::log_level level, const std::string &message);
	};
	cgi_logger logger;
	on_request_handler request_handler;
	on_exception_handler exception_handler;
};

} //namespace

#undef IMPLEMENT_APP
#define IMPLEMENT_APP(app)													\
int main(int argc, char *argv[], char **env) {								\
	return app.run(argc, argv, env);										\
};

#endif /*_CGI_APPLICATION_H_*/
