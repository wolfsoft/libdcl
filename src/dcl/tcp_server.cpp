/*
 * tcp_server.cpp
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

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#include <algorithm>
#include <string.h>

#include <dcl/strutils.h>
#include <dcl/tcp_server.h>

namespace dbp {

#define IO_BUF_SIZE 1500
#define TIMEOUT 300

using namespace std;

tcp_server::tcp_server(size_t worker_threads, size_t queue_size):
  _timeout(TIMEOUT), is_stopped(true), _worker_threads(worker_threads),
  _queue_size(queue_size), _event(_lock) {
	// setup listening thread
	lt.on_execute(create_delegate(this, &tcp_server::io_process));
	// pre-create working threads
	thread t;
	t.on_execute(create_delegate(this, &tcp_server::working_process));
	wkt.resize(worker_threads, t);
}

tcp_server::~tcp_server() {
	stop();
}

void tcp_server::start(const std::string &bind) {
	// do nothing if server is already started
	{
		mutex_guard m(_lock);
		if (!is_stopped)
			return;
		is_stopped = false;
	}
	// parse bind parameter
	strings addrs = tokenize()(bind, ",;");
	listen_sockets.clear();
	for (strings::const_iterator i = addrs.begin(); i != addrs.end(); ++i) {
		tcp_socket s;
		s.bind(*i);
		listen_sockets.push_back(s);
	}
	// start working threads
	for (threads::iterator it = wkt.begin(); it != wkt.end(); ++it)
		it->start();
	// start listening thread
	lt.start();
}

void tcp_server::stop() {
	{
		mutex_guard m(_lock);
		// check if we are already stopped
		if (is_stopped)
			return;
		// set flag to stop threads
		is_stopped = true;
	}
	// notify threads to stop
	_event.raise();
	// wait for listen thread terminates
	lt.wait_for();
	// wait for worker threads terminate
	for (threads::iterator it = wkt.begin(); it != wkt.end(); ++it)
		it->wait_for();
	// close all listen sockets
	listen_sockets.clear();
}

bool tcp_server::is_running() {
	mutex_guard m(_lock);
	return !is_stopped;
}

void tcp_server::disconnect_client(request *rq) {
	if (disconnect_handler)
		disconnect_handler(*rq->connection);
	delete rq;
}

void tcp_server::io_process(thread_int&) {
	// turn socket to a listen state
	for (sockets::iterator i = listen_sockets.begin();
	  i != listen_sockets.end(); ++i)
		i->listen();
	while (1) {
		// check for stopping flag
		{
			mutex_guard m(_lock);
			if (is_stopped)
				break;
		}
		// prepare for socket event
		fd_set rfds, wfds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		// the max socket's handle value for polling
		int n = -1;
		// add listening sockets as event source
		for (sockets::const_iterator i = listen_sockets.begin();
		  i != listen_sockets.end(); ++i) {
		  	// subscribe into 'connect' event
			FD_SET(i->handle(), &rfds);
			n = std::max(i->handle(), n);
		}
		// add active sockets as event source
		{
			mutex_guard m(_lock);
			active_requests::iterator i = a_reqs.begin();
			while (i != a_reqs.end()) {
				// check for client disconnection
				if (((*i)->cur_state == request::CLOSING) &&
				  (*i)->write_buffer.eof()) {
					disconnect_client(*i);
					i = a_reqs.erase(i);
					continue;
				}
				// subscribe into 'ready to read' event
				FD_SET((*i)->connection->handle(), &rfds);
				// subscribe into 'ready to write' event if needed
				if (!(*i)->write_buffer.eof())
					FD_SET((*i)->connection->handle(), &wfds);
				n = std::max((*i)->connection->handle(), n);
				++i;
			}
		}
		// set timeout
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		// poll of socket events
		int r = ::select(n + 1, &rfds, &wfds, NULL, &tv);
		// in case of error
		if (r < 0)
			throw socket_exception(_("can't initialize input/output thread"));
		try {
			// repeat when no more events (timeout)
			if (r == 0)
				continue;
			// check for the event is raised on the listening sockets
			for (sockets::iterator i = listen_sockets.begin();
			  i != listen_sockets.end(); ++i) {
				if (FD_ISSET(i->handle(), &rfds)) {
					FD_CLR(i->handle(), &rfds);
					connection_accept(*i);
				}
			}
			// check for the event is raised on the active sockets
			{
				mutex_guard m(_lock);
				active_requests::iterator i = a_reqs.begin();
				while (i != a_reqs.end()) {
					bool was_events = false;
					// on 'ready to write' event
					if (FD_ISSET((*i)->connection->handle(), &wfds)) {
						FD_CLR((*i)->connection->handle(), &wfds);
						connection_write(**i);
						was_events = true;
					}
				  	// on 'ready to read' event
					if (FD_ISSET((*i)->connection->handle(), &rfds)) {
						FD_CLR((*i)->connection->handle(), &rfds);
						connection_read(**i);
						was_events = true;
					}
					if (was_events && ((*i)->cur_state != request::CLOSING)) {
						// add the request to the processing queue
						reqs.push(*i);
						i = a_reqs.erase(i);
						// wake up the processing thread
						_event.raise();
					} else {
						++i;
					}
				} // while
			}
		}
		catch(dbp::exception &e) {
			// when custom exception handler is assigned, raise the event
			if (exception_handler)
				exception_handler(e);
			else
				throw;
		}
	} // while
}

void tcp_server::connection_accept(socket &ls) {
	// check for maximum stack size; do not accept new
	// connections on overload
	{
		mutex_guard m(_lock);
		if (a_reqs.size() > _queue_size)
			return;
	}
	// accept the connection
	socket s = ls.accept();
	request *rq = NULL;
	if (!create_io_handler)
		rq = new request(new socket(s));
	else {
		socket *rs = create_io_handler(ls, s);
		rq = new request(rs);
	}
	// raise 'on_connect' event
	if (connect_handler) {
		connect_handler(*rq->connection, rq->read_buffer,
		  rq->write_buffer);
	}
	// put the connection into the queue
	{
		mutex_guard m(_lock);
		a_reqs.push_back(rq);
	}
}

void tcp_server::connection_read(request &rq) {
	// exit when no custom event handler assigned
	if (!process_data_handler)
		return;
	//read some data and place into a buffer
	char tmp[IO_BUF_SIZE];
	int size = rq.connection->read(sizeof(tmp), tmp);
	if (size == socket::io_error) {
		rq.read_buffer.setstate(ios::badbit);
		rq.cur_state = request::CLOSING;
	}
	else if (size > 0) {
		// initialize buffers
		rq.write_buffer.clear();
		rq.read_buffer.clear();
		rq.read_buffer.write(tmp, size);
		// raise 'on_process_data' event
		if (!process_data_handler(*rq.connection,
		  rq.read_buffer, rq.write_buffer))
			rq.cur_state = request::CLOSING;
		// discard the read buffer
		if (rq.read_buffer.tellg() ==
		  rq.read_buffer.tellp())
			rq.read_buffer.str(string());
	}
}

void tcp_server::connection_write(request &rq) {
	// exit on empty buffer
	if (rq.write_buffer.eof())
		return;
	// get data size
	int pos = rq.write_buffer.tellg();
	rq.write_buffer.seekg(0, ios::end);
	int size = int(rq.write_buffer.tellg()) - pos;
	if (size == 0) {
		rq.write_buffer.setstate(ios::eofbit);
	} else {
		rq.write_buffer.seekg(pos);
		// allocate buffer and read data
		char tmp[size];
		rq.write_buffer.read(tmp, size);
		rq.write_buffer.seekg(pos);
		// write buffer to the socket
		int rsize = rq.connection->write(size, tmp);
		if (rsize == socket::io_error)
			rq.write_buffer.setstate(ios::badbit);
		else if (rsize > 0) {
			// if the buffer is empty, all data is written,
			// discard the buffer
			if (size == rsize)
				rq.write_buffer.str(string());
			else {
				rq.write_buffer.clear();
				rq.write_buffer.seekg(rsize, ios_base::cur);
			}
		}
	}
}

/*
void tcp_server::working_process(thread_int&) {
			if (r == 0) {
				if (rq->cur_state != request::TIME_OUT) {
					// update last access time counter
					rq->last_access = datetime().now();
					rq->last_state = rq->cur_state;
					rq->cur_state = request::TIME_OUT;
				} else {
					// when timeout is more than 10 secs, disconnect
					if (datetime().now() - rq->last_access > _timeout) {
						disconnect_client(rq);
						continue;
					}
				}
				back_to_queue(rq);
				continue;
			}
			// update last access time counter
			rq->cur_state = rq->last_state;
			rq->last_access = datetime().now();
	}
}
*/

void tcp_server::working_process(thread_int&) {
	while (1) {
		request *rq = NULL;
		try {
			// take the connection from the ready-to-process
			// connections pool
			{
				mutex_guard m(_lock);
				while (reqs.empty() && !is_stopped)
					_event.wait();
				if (is_stopped)
					break;
				rq = reqs.front();
				reqs.pop();
			}
            // process the connection
			if (process_data_handler) {
				if (!process_data_handler(*rq->connection,
				  rq->read_buffer, rq->write_buffer)) {
					rq->cur_state = request::CLOSING;
				}
			}
			// return connection to the active connections list
			{
				mutex_guard m(_lock);
				a_reqs.push_back(rq);
			}
		}
		catch(dbp::exception &e) {
			if (exception_handler)
				exception_handler(e);
			else
				throw;
		}
	}
}

} // namespace
