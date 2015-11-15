/*
 * apache_application.h
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

#ifndef _APACHE_APPLICATION_H_
#define _APACHE_APPLICATION_H_

#include <string>

#include <httpd.h>
#include <http_config.h>
#include <http_core.h>
#include <http_protocol.h>

#include "dcl/delegate.h"
#include "dcl/http_header.h"
#include "dcl/logger.h"
#include "dcl/web_application.h"

namespace dbp {

namespace local {

// Apache helper class interface
class apache_helper {
public:
	virtual const char* get_server_banner() = 0;
	virtual void set_content_type(const char*, request_rec*) = 0;
	virtual void input(size_t*, char**, request_rec*) = 0;
	virtual void output(int, const char*, request_rec*) = 0;
};

#ifndef _LIBRARY_
// Apache helper class implementation
class apache_helper_impl: public apache_helper {
public:
	virtual const char* get_server_banner() {
		return ap_get_server_banner();
	};
	virtual void set_content_type(const char *type, request_rec *r) {
		ap_set_content_type(r, type);
	};
	virtual void input(size_t *len, char **str, request_rec *r) {
		*len = 0;
		apr_bucket_brigade *bb_in = apr_brigade_create(r->pool,
		  r->connection->bucket_alloc);
		if (!bb_in) return;
		bool seen_eos = false;
		do {
			apr_status_t rc = ap_get_brigade(r->input_filters, bb_in,
			  AP_MODE_READBYTES, APR_BLOCK_READ, HUGE_STRING_LEN);
			if (rc != APR_SUCCESS) {
				return;
			}
			for (apr_bucket *bucket = APR_BRIGADE_FIRST(bb_in);
			  bucket != APR_BRIGADE_SENTINEL(bb_in);
			  bucket = APR_BUCKET_NEXT(bucket)) {
				const char *buf;
				apr_size_t buflen;
				rc = apr_bucket_read(bucket, &buf, &buflen, APR_BLOCK_READ);
				if (rc != APR_SUCCESS) {
                	return;
				}
				if (APR_BUCKET_IS_EOS(bucket))
					seen_eos = true;
				*len += buflen;
				*str = (char*)realloc(*str, *len);
				memcpy(*str + *len - buflen, buf, buflen);
			}
			apr_brigade_cleanup(bb_in);
		} while (!seen_eos);
	};
	virtual void output(int len, const char *str, request_rec *r) {
		ap_rwrite(str, len, r);
	};

	local::apache_helper *apache;
};
#endif

} // namespace local

//!	Apache application class
/*!
	The <a href="http://httpd.apache.org">apache web server</a> is a most
	popular web server in the world. It supports extending with an own API by
	plugins (apache modules), which are dynamic libraries by nature.

	This class implements the web aplication as the apache module
	(extension of the apache web server).
	
	See the 'examples/hello_world/apache_module' directory for the apache
	module skeleton application based on apache_application class.
*/
class apache_application: public web_application_int {
public:
	virtual void on_handle_request(on_request_handler handler) {
		request_handler = handler;
	};
	virtual void on_exception(on_exception_handler handler) {
		exception_handler = handler;
	};
	//! Returns logger
	virtual dbp::logger& get_logger() {
		return logger;
	}
	static void apache_register_hooks(apr_pool_t*);
protected:
	virtual http_request get_request(local::apache_helper *apache, request_rec *apache_req) const;
	virtual void send_response(local::apache_helper *apache, request_rec *apache_req, const http_response &response) const;
	virtual http_request get_request() { return http_request(); };
	virtual void send_response(const http_response&) { };
private:
	class apache_logger: public dbp::logger {
	public:
		virtual void log(log_level::log_level level, const std::string &message);
	};

	on_request_handler request_handler;
	on_exception_handler exception_handler;
	apache_logger logger;

	static int apache_handler(request_rec *r);
	int handle_request(local::apache_helper *apache, request_rec *apache_req);
};

} //namespace

#undef IMPLEMENT_APP
//! The define documentation
#define IMPLEMENT_APP(module_name, app_class)								\
																			\
class _apache_app: public app_class, public dbp::singleton<_apache_app> {	\
	friend class dbp::singleton<_apache_app>;								\
};																			\
																			\
dbp::local::apache_helper_impl apache;										\
																			\
static void app_init(apr_pool_t *p, server_rec *s) {						\
	_apache_app::instance();												\
}																			\
																			\
int dbp::apache_application::apache_handler(request_rec *r) {				\
	if (strcmp(r->handler, #module_name) != 0) return DECLINED;				\
	app_class &app = _apache_app::instance();								\
	return app.application_impl().handle_request(&apache, r);				\
}																			\
																			\
void dbp::apache_application::apache_register_hooks(apr_pool_t*) {			\
	ap_hook_child_init(app_init, NULL, NULL, APR_HOOK_MIDDLE);              \
	ap_hook_handler(dbp::apache_application::apache_handler, NULL, NULL,	\
	  APR_HOOK_LAST);														\
}																			\
																			\
extern "C" {																\
																			\
module AP_MODULE_DECLARE_DATA module_name = {								\
	STANDARD20_MODULE_STUFF, 												\
	NULL,						/* create per-dir config structures		*/	\
	NULL,						/* merge  per-dir config structures		*/	\
	NULL,						/* create per-server config structures	*/	\
	NULL,						/* merge  per-server config structures	*/	\
	NULL,						/* table of config file commands		*/	\
	dbp::apache_application::apache_register_hooks /* register hooks	*/	\
};																			\
																			\
}

#endif /*_APACHE_APPLICATION_H_*/
