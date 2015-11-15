/*
 * isapi_application.cpp
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2009 Dennis Prochko <wolfsoft@mail.ru>
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

#include <dcl/isapi_application.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

#define DEFAULT_BUFFER_SIZE 1024

std::string isapi_application::get_server_variable(const std::string &name) const {
	DWORD len = DEFAULT_BUFFER_SIZE;
	CHAR buf[DEFAULT_BUFFER_SIZE];
	CHAR *buf_ptr = buf;
	while (true) {
		if (!isapi_req->GetServerVariable(isapi_req->ConnID, (CHAR*)name.c_str(),
		  buf_ptr, &len)) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buf_ptr != buf)
					throw bad_alloc();
				buf_ptr = new CHAR[len];
				continue;
			} else
				throw dbp::exception(_("The specified variable doesn't exist"));
		} else
			break;
	}
	string rslt(buf_ptr);
	if (buf_ptr != buf)
		delete buf_ptr;
	return rslt;
}

int isapi_application::handle_request() {
	if (!request_handler)
		return HSE_STATUS_ERROR;
	http_request req = get_request();
	int rslt;
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
	if (resp.get_status_code() == http_error::internal_server_error)
		rslt = HSE_STATUS_ERROR;
	else
		rslt = HSE_STATUS_SUCCESS;
	send_response(resp);
	return rslt;
}

http_request isapi_application::get_request() {
	http_request req;
	// obtain client http headers
	strings h = tokenize()(get_server_variable("ALL_RAW"), "\n");
	for (strings::const_iterator i = h.begin(); i != h.end(); ++i)
		req.set_header(trim()(*i));
	// obtain request parameters
	req.set_method(to_string<const char*>(isapi_req->lpszMethod));
	req.set_query_string(to_string<const char*>(isapi_req->lpszQueryString));
	req.set_path_info(to_string<const char*>(isapi_req->lpszPathInfo));
	req.set_path_translated(to_string<const char*>(isapi_req->lpszPathTranslated));
	req.set_content_type(to_string<const char*>(isapi_req->lpszContentType));
	req.set_auth_type(get_server_variable("AUTH_TYPE"));
	req.set_remote_user(get_server_variable("REMOTE_USER"));
	req.set_script_name(get_server_variable("SCRIPT_NAME"));
	req.set_server_name(get_server_variable("SERVER_NAME"));
	req.set_server_port(from_string<int>(get_server_variable("SERVER_PORT")));
	req.set_server_protocol(get_server_variable("SERVER_PROTOCOL"));
	req.set_server_software(get_server_variable("SERVER_SOFTWARE"));
	req.set_remote_host(get_server_variable("REMOTE_HOST"));
	req.set_remote_addr(get_server_variable("REMOTE_ADDR"));
	req.set_gateway_interface(get_server_variable("GATEWAY_INTERFACE"));
	// read request body
	if (isapi_req->cbTotalBytes > 0) {
		// copy currently fetched data
		char *buf = (char*)malloc(isapi_req->cbTotalBytes);
		memcpy(buf, isapi_req->lpbData, isapi_req->cbAvailable);
		// copy a rest of data
		if (isapi_req->cbTotalBytes > isapi_req->cbAvailable) {
			DWORD len = isapi_req->cbTotalBytes - isapi_req->cbAvailable;
			if (!isapi_req->ReadClient(isapi_req->ConnID,
			  buf + isapi_req->cbAvailable, &len)) {
				free(buf);
				throw dbp::exception(
				  _("Can't obtain data from the web server"));
			}
		}
		req.set_content(isapi_req->cbTotalBytes, buf);
		free(buf);
	}
	return req;
}

void isapi_application::send_response(const http_response &response) {
	string h;
	// make a string with all response http headers
	for (http_response::http_headers::const_iterator i =
	  response.all_headers().begin(); i != response.all_headers().end(); ++i)
		h += i->first + ": " + i->second + CRLF;
	// add all response cookies
	for (http_cookies::const_iterator i = response.get_cookies().begin();
	  i != response.get_cookies().end(); ++i) {
		h += "Set-Cookie: " + i->str() + CRLF;
	}
	h += CRLF;
	// send headers to a client
	HSE_SEND_HEADER_EX_INFO hdr;
	hdr.pszStatus = response.get_status().c_str();
	hdr.pszHeader = h.c_str();
	hdr.cchStatus = response.get_status_code();
	hdr.cchHeader = h.length();
	hdr.fKeepConn = false;
	isapi_req->ServerSupportFunction(isapi_req->ConnID,
	  HSE_REQ_SEND_RESPONSE_HEADER_EX, &hdr, NULL, NULL);
	// if there is a content, send it too	
	DWORD len = response.get_content_size();
	if (len > 0)
		if (!isapi_req->WriteClient(isapi_req->ConnID,
		  (void*)response.get_content(), &len, 0))
			throw dbp::exception(
			  _("Can't send data to the web server"));
}

} // namespace

