/*
 * tcp_client.h
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

#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

//#include <queue>
#include <string>
#include <iostream>

//#include <dcl/datetime.h>
#include <dcl/delegate.h>
//#include <dcl/event.h>
//#include <dcl/mutex.h>
#include <dcl/socket.h>
//#include <dcl/thread.h>

namespace dbp {

//!	TCP Client class
/*!
	This class provides generic TCP client features: it connects to
	specified server, sends and receives data. All networking
    details are hidden from the user; the interaction with the
    server is doing by events.
	
	The client is asynchronous and multithreaded.
*/
class tcp_client {
public:
	typedef delegate1<socket&, bool> on_before_connect_handler;
	typedef delegate3<const socket&, std::istream&, std::ostream&, void>
	  on_after_connect_handler;
	typedef delegate1<socket&, void> on_disconnect_handler;
	typedef delegate3<const socket&, std::istream&, std::ostream&,
	  bool> on_process_data_handler;
	typedef delegate1<const dbp::exception&, void> on_exception_handler;
	//! Constructor
/*	tcp_server(size_t worker_threads = 2, size_t queue_size = 32768);
	//! Destructor
	~tcp_server();*/
	//! Get timeout value
	int timeout() {
		return _timeout;
	}
	//! Set timeout value
	tcp_client& timeout(int value) {
		_timeout = value;
		return *this;
	}
	//! Connect to server
	/*!
		TODO description needed
	*/
	//void connect(const std::string &address);
	//! Assign 'on_before_connect' event handler
	/*!
		Your class can assign the event handler to the tcp_client
		on_before_connect event. The handler is called by tcp_client class
		when the client connects to server.
		
		\param handler the event handler delegate
		
		\see
		\ref delegates
	*/
	void on_before_connect(on_before_connect_handler handler) {
		before_connect_handler = handler;
	}
	void on_after_connect(on_after_connect_handler handler) {
		after_connect_handler = handler;
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
/*	// Parameters
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
		request(): cur_state(WAIT_DATA), last_state(WAIT_DATA) {
			read_buffer.setstate(std::ios::eofbit);
			write_buffer.setstate(std::ios::eofbit);
		}
		request(socket &conn): cur_state(WAIT_DATA), last_state(WAIT_DATA),
		  connection(conn) {
			read_buffer.setstate(std::ios::eofbit);
			write_buffer.setstate(std::ios::eofbit);
		}
		states cur_state, last_state;
		socket connection;
		std::stringstream read_buffer, write_buffer;
		datetime last_access;
	};
	typedef std::queue<request*> requests;
	requests reqs;
	// Worker threads
	typedef std::vector<thread> threads;
	threads wkt;
	// Synchronization
	mutex _lock;
	event _event;*/
	// Custom handlers
	on_before_connect_handler before_connect_handler;
	on_after_connect_handler after_connect_handler;
	on_disconnect_handler disconnect_handler;
	on_process_data_handler process_data_handler;
	on_exception_handler exception_handler;
/*	// Process handlers
	void listen_process(thread_int&);
	void working_process(thread_int&);
	// Utility functions
	void back_to_queue(request*);
	void disconnect_client(request*);*/
};

}

#endif /*_TCP_SERVER_H_*/
