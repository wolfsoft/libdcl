/*
 * apache_application.cpp
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

#define _LIBRARY_
#include "dcl/apache_application.h"
#undef _LIBRARY_
#include "dcl/http_header.h"
#include "dcl/strutils.h"

#include <apr_strings.h>

#ifdef AP_DECLARE_MODULE
#define APACHE24
#endif

namespace dbp {

using namespace std;

static int fill_request(void *data, const char *key, const char *value) {
	if (data && key && value)
		static_cast<http_request*>(data)->set_header(key, value);
	return true;
}

int apache_application::handle_request(local::apache_helper *apache,
  request_rec *apache_req) {
	if (!request_handler)
		return DECLINED;
	const http_request &req = get_request(apache, apache_req);
	int rslt;
	http_response resp;
	try {
		resp = request_handler(req);
	} catch (dbp::exception &e) {
		if (exception_handler)
			exception_handler(e, resp);
		else {
			// return 500 error to the client
			resp.set_status(http_error::internal_server_error);
			resp.set_content(e.what());
			get_logger().error(e.what());
		}
	}
	catch (std::exception &e) {
		// return 505 error to the client
		resp.set_status(http_error::internal_server_error);
		resp.set_content(e.what());
		get_logger().error(e.what());
	}
	rslt = resp.get_status_code();
	if (rslt == http_error::ok)
		rslt = OK;
	send_response(apache, apache_req, resp);
	return rslt;
}

http_request apache_application::get_request(local::apache_helper *apache,
  request_rec *apache_req) const {
	http_request req;
	// read request headers
	apr_table_do(fill_request, &req, apache_req->headers_in, NULL);
	req.set_auth_type(to_string<const char*>(apache_req->ap_auth_type));
	req.set_remote_user(to_string<const char*>(apache_req->user));
	req.set_path_translated(to_string<const char*>(apache_req->filename));
	req.set_method(to_string<const char*>(apache_req->method));
	req.set_path_info(to_string<const char*>(apache_req->path_info));
	req.set_query_string(to_string<const char*>(apache_req->args));
	req.set_script_name(to_string<const char*>(apache_req->uri));
	req.set_server_name(to_string<const char*>(apache_req->server->server_hostname));
	req.set_server_port(apache_req->server->port);
	req.set_server_protocol(to_string<const char*>(apache_req->protocol));
	req.set_server_software(to_string<const char*>(apache->get_server_banner()));
	req.set_remote_host(to_string<const char*>(apache_req->connection->remote_host));
#ifdef APACHE24
	req.set_remote_addr(to_string<const char*>(apache_req->connection->client_ip));
#else
	req.set_remote_addr(to_string<const char*>(apache_req->connection->remote_ip));
#endif
	req.set_gateway_interface("CGI/1.1");
	// read request body
	size_t len = 0;
	char *buffer = NULL;
	apache->input(&len, &buffer, apache_req);
	if (len > 0)
		req.set_content(len, buffer);
	if (buffer)
		free(buffer);
	return req;
}

void apache_application::send_response(local::apache_helper *apache,
  request_rec *apache_req, const http_response &response) const {
	// set all headers
	http_response::http_headers::const_iterator i = response.all_headers().begin();
	while (i != response.all_headers().end()) {
		if ((i->first != string("Content-Length")) &&
		    (i->first != string("Content-Type")))
			apr_table_set(apache_req->headers_out, i->first.c_str(), i->second.c_str());
		++i;
	}
	const http_cookies &c = response.get_cookies();
	for (http_cookies::const_iterator i = c.begin(); i != c.end(); ++i) {
		apr_table_set(apache_req->headers_out, "Set-Cookie", i->str().c_str());
	}
	// set other response variables
	apache_req->status = response.get_status_code();
	if (!response.get_content_type().empty())
		apache->set_content_type(apr_pstrdup(apache_req->pool,
		  response.get_content_type().c_str()), apache_req);
	// set response content
	if (response.get_content_size() > 0)
		apache->output(response.get_content_size(), response.get_content(),
		  apache_req);
}

void apache_application::apache_logger::log(log_level::log_level level, const std::string &message) {
	cerr << message << endl << flush;
}

} // namespace
