/*
 * cgi_application.cpp
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

#ifdef _WIN32
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#endif

#include "dcl/cgi_application.h"
#include "dcl/strutils.h"

namespace dbp {

using namespace std;

http_request cgi_application::get_request(char **env) {
	http_request req;
	int c_len = 0;
	// read environment variables
	while (env && *env) {
		// split PARAM=VALUE of the environment
		string line(*env), left, right;
		string::size_type pos = line.find_first_of("=");
		if (pos == string::npos) {
			left = line;
		} else {
			left = line.substr(0, pos);
			right = line.substr(pos + 1, line.length() - pos - 1);
		}
		// analyze PARAM
		if (left == "AUTH_TYPE")
			req.set_auth_type(right);
		else
		if (left == "CONTENT_LENGTH")
			c_len = from_string<int>(right);
		else
		if (left == "CONTENT_TYPE")
			req.set_content_type(right);
		else
		if (left == "GATEWAY_INTERFACE")
			req.set_gateway_interface(right);
		else
		if (left == "PATH_INFO")
			req.set_path_info(right);
		else
		if (left == "PATH_TRANSLATED")
			req.set_path_translated(right);
		else
		if (left == "QUERY_STRING")
			req.set_query_string(right);
		else
		if (left == "REMOTE_ADDR")
			req.set_remote_addr(right);
		else
		if (left == "REMOTE_HOST")
			req.set_remote_host(right);
		else
		if (left == "REMOTE_IDENT")
			req.set_remote_ident(right);
		else
		if (left == "REMOTE_USER")
			req.set_remote_user(right);
		else
		if (left == "REQUEST_METHOD")
			req.set_method(right);
		else
		if (left == "SCRIPT_NAME")
			req.set_script_name(right);
		else
		if (left == "SERVER_NAME")
			req.set_server_name(right);
		else
		if (left == "SERVER_PORT")
			req.set_server_port(from_string<int>(right));
		else
		if (left == "SERVER_PROTOCOL")
			req.set_server_protocol(right);
		else
		if (left == "SERVER_SOFTWARE")
			req.set_server_software(right);
		else {
			// is it a HTTP header?
			if (left.compare(0, 5, "HTTP_") == 0) {
				left = left.substr(5, left.length() - 5);
				string::size_type pos = left.find_first_of("_");
				while (pos != string::npos) {
					left[pos] = '-';
					pos = left.find_first_of("_");
				}
				req.set_header(left, right);
			}
		}
		// check next environment variable
		env++;
	}
	// if there is a content, read it from standard input
	if (c_len > 0)
		req.add_content(c_len, cin);
	return req;
}

http_request cgi_application::get_request() {
	return get_request(NULL);
}

void cgi_application::send_response(const http_response &response) {
#ifdef _WIN32
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	cout << "Status: " << response.get_status() << CRLF;
	cout << response;
}

int cgi_application::run(int, char*[], char **env) {
	http_request req = get_request(env);
	if (request_handler) {
		http_response resp;
		try {
			resp = request_handler(req);
		} catch (std::exception &e) {
			if (exception_handler)
				exception_handler(e, resp);
			else {
				resp.set_status(http_error::internal_server_error);
				resp.set_content(e.what());
			}
		}
		send_response(resp);
	}
	return 0;
}

void cgi_application::cgi_logger::log(log_level::log_level level, const std::string &message) {
	cerr << message << endl << flush;
}

} // namespace

