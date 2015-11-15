/*
 * isapi_application.h
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

#ifndef _ISAPI_APPLICATION_H_
#define _ISAPI_APPLICATION_H_

#include <httpext.h>

#include <dcl/delegate.h>
#include <dcl/http_header.h>
#include <dcl/web_application.h>

namespace dbp {

//!	ISAPI application class
/*!
	This class implements the web application as the ISAPI module
	(Microsoft IIS web server extension).
	
	The <a href="http://www.microsoft.com/iis">IIS</a> is a somewhat
	popular web server in the Windows environment. It supports extending
	with an own API by plugins (ISAPI modules), which are dynamic
	libraries by nature.
	
	This class provides a wrapper on the isapi modules API, so you can
	use this class and link your program as dynamic library to create
	your isapi module extension:
	
	\include hello_world/isapi_module/main.cpp
	
	See the 'examples/hello_world/isapi_module' directory for the full ISAPI
	module skeleton application based on isapi_application class.
*/
class isapi_application: public web_application_int {
	friend DWORD ::HttpExtensionProc(EXTENSION_CONTROL_BLOCK*);
public:
	isapi_application(): isapi_req(NULL) { };
	virtual void on_handle_request(on_request_handler handler) {
		request_handler = handler;
	};
	virtual void on_exception(on_exception_handler handler) {
		exception_handler = handler;
	};
protected:
	int handle_request();
	virtual http_request get_request();
	virtual void send_response(const http_response &response);
private:
	LPEXTENSION_CONTROL_BLOCK isapi_req;
	on_request_handler request_handler;
	on_exception_handler exception_handler;
	std::string get_server_variable(const std::string &name) const;
};

} //namespace

#undef IMPLEMENT_APP
#define IMPLEMENT_APP(module_name, app_class)								\
extern "C" {																\
																			\
__declspec(dllexport)														\
BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO* pVer) {					\
	pVer->dwExtensionVersion =												\
	  MAKELONG(HSE_VERSION_MINOR,HSE_VERSION_MAJOR);						\
	strncpy(pVer->lpszExtensionDesc, #module_name,							\
	  HSE_MAX_EXT_DLL_NAME_LEN);											\
	return true;															\
}																			\
																			\
__declspec(dllexport)														\
BOOL WINAPI TerminateExtension(DWORD dwFlags) {								\
	return true;															\
}																			\
																			\
__declspec(dllexport)														\
DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB) {			\
	app_class app;															\
	dbp::isapi_application &_app = app.application_impl();					\
	_app.isapi_req = lpECB;													\
	return _app.handle_request();											\
}																			\
																			\
} //extern

#endif /*_ISAPI_APPLICATION_H_*/
