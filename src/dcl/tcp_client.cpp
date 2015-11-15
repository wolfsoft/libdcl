/*
 * tcp_client.cpp
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

#include <dcl/tcp_client.h>

namespace dbp {

#define IO_BUF_SIZE 1500
#define TIMEOUT 300

using namespace std;


/*
tcp_client::tcp_client(size_t worker_threads, size_t queue_size):
  _timeout(TIMEOUT), is_stopped(true), _worker_threads(worker_threads),
  _queue_size(queue_size), _event(_lock) {
	// setup listening thread
	lt.on_execute(create_delegate(this, &tcp_client::listen_process));
	// pre-create working threads
	thread t;
	t.on_execute(create_delegate(this, &tcp_client::working_process));
	wkt.resize(worker_threads, t);
}


tcp_client::~tcp_client() {
	stop();
}

void tcp_client::start(const std::string &bind) {
	// do nothing if client is already started
	{
		mutex_guard m(_lock);
		if (!is_stopped)
			return;
		is_stopped = false;
	}
	// parse bind parameter
	strings addrs = tokenize()(bind, ";");
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

void tcp_client::stop() {
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

bool tcp_client::is_running() {
	mutex_guard m(_lock);
	return !is_stopped;
}

bool cmp_sockets(const socket &s1, const socket &s2) {
	return s1.handle() < s2.handle();
}

void tcp_client::listen_process(thread_int&) {
	fd_set fds;
	FD_ZERO(&fds);
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
		// wait for incoming connections
		for (sockets::const_iterator i = listen_sockets.begin();
		  i != listen_sockets.end(); ++i) {
			FD_SET(i->handle(), &fds);
		}
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int r = ::select(std::max_element(listen_sockets.begin(),
		  listen_sockets.end(), cmp_sockets)->handle() + 1, &fds,
		  NULL, NULL, &tv);
		if (r < 0)
			throw socket_exception(_("can't initialize listen thread"));
		if (r > 0) {
			// should we accept incoming connection?
			for (sockets::iterator i = listen_sockets.begin();
			  i != listen_sockets.end(); ++i) {
				if (FD_ISSET(i->handle(), &fds)) {
					// yes, we should
					FD_CLR(i->handle(), &fds);
					// check for maximum stack size; do not accept new
					// connections on overload
					{
						mutex_guard m(_lock);
						if (reqs.size() > _queue_size)
							continue;
					}
					// raise 'on_before_connect' event
					if (before_connect_handler)
						if (!before_connect_handler(*i))
							continue;
					// accept the connection
					socket s = i->accept();
					request *rq = new request(s);
					// raise 'on_after_connect' event
					if (after_connect_handler)
						after_connect_handler(s, rq->read_buffer,
						  rq->write_buffer);
					// put the connection into the queue
					{
						mutex_guard m(_lock);
						reqs.push(rq);
					}
					// wake up working threads to process the request
					_event.raise();
				} // if
			} // for
		} // if
	} // while
}

void tcp_client::working_process(thread_int&) {
	fd_set rfds, wfds;
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	while (1) {
		request *rq = NULL;
		try {
			// take the socket from the pool
			{
				mutex_guard m(_lock);
				while (reqs.empty() && !is_stopped)
					_event.wait();
				if (is_stopped)
					break;
				rq = reqs.front();
				reqs.pop();
			}
			// wait for ready to read or write
			FD_SET(rq->connection.handle(), &rfds);
			if (!rq->write_buffer.eof())
				FD_SET(rq->connection.handle(), &wfds);
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			int r = ::select(rq->connection.handle() + 1,
			  &rfds, &wfds, NULL, &tv);
			if (r < 0)
				throw socket_exception(_("can't initialize working thread"));
			// timeout; process other waiting tasks
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
			// data is ready to read?
			if (FD_ISSET(rq->connection.handle(), &rfds)) {
				FD_CLR(rq->connection.handle(), &rfds);
				if (process_data_handler) {
					//read some data and place into a buffer
					char tmp[IO_BUF_SIZE];
					int size = rq->connection.read(sizeof(tmp), tmp);
					if (size < 0)
						rq->read_buffer.setstate(ios::badbit);
					else if (size == 0)
						rq->read_buffer.setstate(ios::eofbit);
					else {
						// initialize buffers
						rq->write_buffer.clear();
						rq->read_buffer.clear();
						rq->read_buffer.write(tmp, size);
						// raise 'on_process_data' event
						if (!process_data_handler(rq->connection,
						  rq->read_buffer, rq->write_buffer))
							rq->cur_state = request::CLOSING;
						// discard the read buffer
						if (rq->read_buffer.tellg() ==
						  rq->read_buffer.tellp())
							rq->read_buffer.str(string());
					}
				}
			}
			// data is ready to write?
			if (FD_ISSET(rq->connection.handle(), &wfds)) {
				FD_CLR(rq->connection.handle(), &wfds);
				if (rq->write_buffer.good()) {
					// get data size
					int pos = rq->write_buffer.tellg();
					rq->write_buffer.seekg(0, ios::end);
					int size = int(rq->write_buffer.tellg()) - pos;
					if (size == 0) {
						rq->write_buffer.setstate(ios::eofbit);
					} else {
						rq->write_buffer.seekg(pos);
						// allocate buffer and read data
						char tmp[size];
						rq->write_buffer.read(tmp, size);
						rq->write_buffer.seekg(pos);
						// write buffer to the socket
						int rsize = rq->connection.write(size, tmp);
						if (rsize < 0)
							rq->write_buffer.setstate(ios::badbit);
						else if (rsize > 0) {
							// if the buffer is empty, all data is written,
							// discard the buffer
							if (size == rsize)
								rq->write_buffer.str(string());
							else {
								rq->write_buffer.clear();
								rq->write_buffer.seekg(rsize, ios_base::cur);
							}
						}
					}
				}
			}
			// close connection if needed
			if ((rq->cur_state == request::CLOSING) &&
			  (!rq->write_buffer.good())) {
				disconnect_client(rq);
				continue;
			}
			// process next task
			back_to_queue(rq);
		}
		catch(dbp::exception &e) {
			if (exception_handler)
				exception_handler(e);
			else
				throw;
		}
	}
}

void tcp_client::back_to_queue(request *rq) {
	mutex_guard m(_lock);
	reqs.push(rq);
	_event.raise();
}

void tcp_client::disconnect_client(request *rq) {
	if (disconnect_handler)
		disconnect_handler(rq->connection);
	delete rq;
}
*/
} // namespace
