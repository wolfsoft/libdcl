/*
 * http_server.cpp
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

#include <algorithm>
#include <iterator>

#include <dcl/delegate.h>
#include <dcl/http_server.h>
#include <dcl/strutils.h>

namespace dbp {

#define IO_BUF_SIZE 1500

using namespace std;

http_server::http_server(size_t worker_threads, size_t queue_size):
  tcp_server::tcp_server(worker_threads, queue_size) {
	on_process_data(create_delegate(this, &http_server::process_data));
}

bool http_server::process_data(const socket &connection,
  std::istream &in, std::ostream &out) {
	// if there is no on_request event handler assigned, exit
	if (!request_handler)
		return false;
	// obtain the request related to socket handle
	request *req = NULL;
	{
		mutex_guard m(lock);
		requests::iterator it = reqs.find(connection.handle());
		if (it != reqs.end())
			req = it->second;
		else
			req = new request();
	}
	// state machine
	while (1) {
		// construct the http request
		switch (req->state) {
			case request::WAIT_COMMAND: {
				// read full line into the buffer
				string buf;
				size_t pos = in.tellg();
				getline(in, buf);
				if (in.eof()) {
					in.seekg(pos);
					{
						mutex_guard m(lock);
						reqs[connection.handle()] = req;
					}
					return true;
				}
				// parse the HTTP command
				pos = buf.find_first_of(' ');
				// Cut out method name
				if (pos != string::npos) {
					req->req.set_method(trim()(buf.substr(0, pos + 1)));
					buf.erase(0, pos + 1);
				}
				// Cut out protocol version
				pos = buf.find_last_of(' ');
				if (pos != string::npos) {
					req->req.http_version(trim()(buf.substr(pos + 1)));
					buf.erase(pos);
				}
				// The remaining is a path_info
				req->req.set_path_info(buf);
				// Next data chunk should be http header
				req->state = request::WAIT_HEADER;
				break;
			}
			case request::WAIT_HEADER: {
				// read full line into the buffer
				string buf;
				int pos = in.tellg();
				getline(in, buf);
				if (in.eof()) {
					in.seekg(pos);
					{
						mutex_guard m(lock);
						reqs[connection.handle()] = req;
					}
					return true;
				}
				req->req.set_header(trim()(buf));
				// when blank line - headers end
				if (trim()(buf).empty()) {
					if (!req->req.get_header("Content-Length").empty()) {
						req->data_size = from_string<size_t>(
						  req->req.get_header("Content-Length"));
					}
					req->state = request::RECEIVING_DATA;
				}
				break;
			}
			case request::RECEIVING_DATA: {
				// if there are data available
				if (req->data_size > 0) {
					if (in.eof()) {
						mutex_guard m(lock);
						reqs[connection.handle()] = req;
						return true;
					}
					size_t pos = in.tellg();
					req->req.add_content(in);
					req->data_readed += in.tellg();
					req->data_readed -= pos;
					if (req->data_readed >= req->data_size)
						req->state = request::PROCESS_REQUEST;
				} else
					req->state = request::PROCESS_REQUEST;
				break;
			}
			case request::PROCESS_REQUEST: {
				if (request_handler) {
					http_response resp = request_handler(req->req);
					out << req->req.http_version() << " " <<
					  resp.get_status() << endl << resp << flush;
				}
				req->state = request::CLOSING;
				break;
			}
			case request::CLOSING: {
				bool keep_alive = (req->req.get_header("Connection") ==
				  "keep-alive");
				{
					mutex_guard m(lock);
					delete req;
					reqs.erase(connection.handle());
				}
				// keep connection on keep-alive
				return keep_alive;
				break;
			}
		} // switch
	} // while
}

} // namespace
