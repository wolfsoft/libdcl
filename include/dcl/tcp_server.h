/*
 * tcp_server.h
 * This file is part of dbPager Server
 *
 * Copyright (c) 2009 Dennis Prochko <wolfsoft@mail.ru>
 *
 * dbPager Server is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * dbPager Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dbPager Server; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <list>
#include <queue>
#include <string>
#include <iostream>

#include <dcl/datetime.h>
#include <dcl/delegate.h>
#include <dcl/event.h>
#include <dcl/mutex.h>
#include <dcl/socket.h>
#include <dcl/thread.h>

namespace dbp {

//!	TCP Server class
/*!
	This class provides generic TCP server features: it listens on
	the address/port specified, accepts new connections and process
	them. All networking details are hidden from the user; the
	interaction with the client is doing by events.
	
	The server is asynchronous and multithreaded: one thread is
	listening the port; working threads are deal with data. There
	is the pool of incoming connections - the maximum number of
	active clients.
*/
class tcp_server {
public:
	typedef delegate2<const socket&, const socket&, socket*>
	  on_io_handler;
	typedef delegate3<const socket&, std::istream&, std::ostream&, void>
	  on_connect_handler;
	typedef delegate1<socket&, void> on_disconnect_handler;
	typedef delegate3<const socket&, std::istream&, std::ostream&,
	  bool> on_process_data_handler;
	typedef delegate1<const dbp::exception&, void> on_exception_handler;
	//! Constructor
	tcp_server(size_t worker_threads = 2, size_t queue_size = 32768);
	//! Destructor
	~tcp_server();
	//! Get timeout value
	int timeout() {
		return _timeout;
	}
	//! Set timeout value
	tcp_server& timeout(int value) {
		_timeout = value;
		return *this;
	}
	//! Start the server
	/*!
		Starts the socket listening and processes the requests. The listening
		will completed on addresses and ports provided by bind parameter in
		the following format:

		host:port;host:port;...

		where host is IP address or host name to listen on, or '*' meaning
		any address and port is the port number, for example:
		"*:80;localhost:443". The zero port meaning random port available
		selected by OS.

		\param bind the address(es) and port(s) to listen on
	*/
	void start(const std::string &bind = "*:0");
	//! Stop the server
	/*!
		Signals to all working tasks to stop and shuts down the server.
	*/
	void stop();
	//! Detect server status
	/*!
		\returns true if the server is working, false if it's stopped.
	*/
	bool is_running();
	//! Assign 'on_create_io' event handler
	/*!
		Your class can assign the event handler to the tcp_server
		on_connect event. The handler is called by tcp_server class
		when the client connects to server.
		
		\param handler the event handler delegate
		
		\see
		\ref delegates
	*/
	void on_create_io_handler(on_io_handler handler) {
		create_io_handler = handler;
	}
	void on_connect(on_connect_handler handler) {
		connect_handler = handler;
	}
	void on_disconnect(on_disconnect_handler handler) {
		disconnect_handler = handler;
	}
	void on_process_data(on_process_data_handler handler) {
		process_data_handler = handler;
	}
	void on_exception(on_exception_handler handler) {
		exception_handler = handler;
	}
private:
	// Options
	int _timeout;
	// Stop flag
	bool is_stopped;
	// Parameters
	size_t _worker_threads;
	size_t _queue_size;
	// Listen sockets
	typedef std::vector<socket> sockets;
	sockets listen_sockets;
	// Listen thread
	thread lt;
	// Requests queue
	struct request {
		enum states {
			WAIT_DATA,
			TIME_OUT,
			CLOSING
		};
		request(socket *conn): cur_state(WAIT_DATA), last_state(WAIT_DATA),
		  connection(conn) {
			read_buffer.setstate(std::ios::eofbit);
			write_buffer.setstate(std::ios::eofbit);
		}
		~request() {
			connection->shutdown();
			delete connection;
		}
		states cur_state, last_state;
		socket *connection;
		std::stringstream read_buffer, write_buffer;
		datetime last_access;
	};
	// Active requests
	typedef std::list<request*> active_requests;
	active_requests a_reqs;
	// Processing requests
	typedef std::queue<request*> requests;
	requests reqs;
	// Worker threads
	typedef std::vector<thread> threads;
	threads wkt;
	// Synchronization
	mutex _lock;
	event _event;
	// Custom handlers
	on_io_handler create_io_handler;
	on_connect_handler connect_handler;
	on_disconnect_handler disconnect_handler;
	on_process_data_handler process_data_handler;
	on_exception_handler exception_handler;
	// Process handlers
	void io_process(thread_int&);
	void working_process(thread_int&);
	// Utility functions
	void connection_accept(socket &s);
	void connection_write(request &r);
	void connection_read(request &r);
	void disconnect_client(request*);
};

}

#endif /*_TCP_SERVER_H_*/
