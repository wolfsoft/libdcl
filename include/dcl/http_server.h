/*
 * http_server.h
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

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <map>
#include <string>
#include <sstream>

#include <dcl/delegate.h>
#include <dcl/http_header.h>
#include <dcl/mutex.h>
#include <dcl/tcp_server.h>

namespace dbp {

//!	HTTP server
/*!
	This class provides a generic HTTP server.
*/
class http_server: public tcp_server {
public:
	//! HTTP request handler
	typedef delegate1<const http_request&, http_response> on_request_handler;
	//! Constructor
	http_server(size_t worker_threads = 2, size_t queue_size = 32768);
	//! Destructor
	virtual ~http_server() { }
	//! Assign HTTP request handler
	void on_request(on_request_handler handler) {
		request_handler = handler;
	}
protected:
	void on_process_data(on_process_data_handler handler) {
		tcp_server::on_process_data(handler);
	}
private:
	// processing requests
	struct request {
		enum states {
			WAIT_COMMAND,
			WAIT_HEADER,
			RECEIVING_DATA,
			PROCESS_REQUEST,
			CLOSING
		};
		request(): state(WAIT_COMMAND), data_size(0), data_readed(0) { }
		states state;
		size_t data_size;
		size_t data_readed;
		http_request req;
	};
	typedef std::map<int, request*> requests;
	requests reqs;
	mutex lock;
	// Handlers
	on_request_handler request_handler;
	// Custom handlers
	bool process_data(const socket&, std::istream&, std::ostream&);
};

} // namespace

#endif /*_HTTP_SERVER_H_*/
