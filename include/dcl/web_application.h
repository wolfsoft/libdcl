/*
 * web_application.h
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

#ifndef _WEB_APPLICATION_H_
#define _WEB_APPLICATION_H_

#include <dcl/delegate.h>
#include <dcl/exception.h>
#include <dcl/http_header.h>
#include <dcl/logger.h>

namespace dbp {

//!	Generic web application interface class
/*!
	This class is a parent for concrete web application classes:
	cgi_application, apache_application and isapi_application.

	Basically you shouldn't use this interface class directly - use the
	concrete web application classes.
*/
class web_application_int {
public:
	//! Exception handler
	typedef delegate2<const std::exception&, http_response&, void>
	  on_exception_handler;
	//! Web request handler
	typedef delegate1<const http_request&, http_response> on_request_handler;
	//! Register a custom web request handler
	/*!
		To make your application doing something useful, you should
		register your web request handler.
		
		This handler is a web application entry point. When client is
		passing the request, it comes to a web server, processed with it
		and delivers to the application. You can obtain web request fields
		via http_request parameter.

		After processing of the request you should to deliver the
		appropriate response to a client via web server by returning
		initialized http_response object.

		The 'examples/hello_world/cgi_application' directory contains the
		source code of the web request handler implementation example.

		\param handler the request handler delegate.
	*/
	virtual void on_handle_request(on_request_handler handler) = 0;
	//! Register a custom exception handler
	/*!
		By default, the web application returns the '500 Internal Server Error'
		HTTP error code on exceptions occured, but you can customize the
		default exceptions handling by registering your handler.

		\param handler the exception handler delegate.
	*/
	virtual void on_exception(on_exception_handler handler) = 0;
	//! Returns logger
	virtual dbp::logger& get_logger() = 0;
protected:
	virtual http_request get_request() = 0;
	virtual void send_response(const http_response&) = 0;
};

} //namespace

#endif /*_WEB_APPLICATION_H_*/
