/*
 * socket.h
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

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <vector>
#include <string>

#include <dcl/exception.h>

namespace dbp {

//!	Socket error exception class.
/*!
	The socket_exception class is used as base class for all socket related
	exceptions.
*/
class socket_exception: public exception {
public:
	//! Constructor
	socket_exception(const std::string &msg = "") noexcept: exception(msg) { }
};

//!	Represents socket's address list and a port
class socket_address: public std::vector<std::string> {
public:
	int port;
	std::string host;
	//! Constructor (default).
	socket_address(): port(0), host("") {};
	//! Constructor (parameterized).
	/*!
		\param addr the socket address in format "address[:port]"
	*/
	socket_address(const std::string &addr) { parse(addr); };
	//! Constructor (parameterized).
	/*!
		\param host the socket ip address or host name
		\param port the socket port number
	*/
	socket_address(const std::string &address, int port): port(port),
	  host(address) { };
	//! Parse the address in format "address[:port]"
	/*!
		\param addr the socket address in format "address[:port]"
	*/
	void parse(const std::string &addr);
};

//!	Base class for network sockets.
/*!
	Detailed class description.
*/
class socket {
public:
	typedef enum {
		data_not_ready = -1,
		io_error = -2
	} error;
	//!	Constructor
	socket();
	//! Constructor
	socket(const socket &src) {
		socket_fd = src.socket_fd;
		_port = src._port;
		_address = src._address;
		const_cast<socket&>(src).socket_fd = -1;
	}
	//! Copy operator
	socket& operator=(const socket &src) {
		socket_fd = src.socket_fd;
		_port = src._port;
		_address = src._address;
		const_cast<socket&>(src).socket_fd = -1;
		return *this;
	}
	//!	Destructor
	virtual ~socket();
	//! Get socket handle
	virtual int handle() const {
		return socket_fd;
	}
	//!	Connection to remote server
	/*!
		\param address a server address you want connect to
	*/
	bool connect(const std::string &address, int port);
	//!	Binding to the specified network interfaces
	/*!
		\param address a network address you want bind to
	*/
	void bind(const socket_address &address);
	//!	Listening for incoming connections
	/*!
		Switchs a socket to a passive state for listening for connections
		from clients.
	*/
	void listen();
	//! Accept the incoming connection
	/*!
		Accepts the incoming connection. The socket should be in listen
		mode to do this.
		
		\returns a socket to communicate
	*/
	socket accept();
	//! Read data from the socket
	/*!
		Reads data from buffer to a socket.
		\param bytes_to_read a size of the buffer (number a bytes to read)
		\param buffer a pointer to data readed
		\return number of a bytes readed or -1 on error
	*/
	virtual int read(int bytes_to_read, char *buffer);
	//! Write data to the socket
	/*!
		Writes buffer to a socket.
		\param bytes_to_write a size of a buffer (number a bytes to write)
		\param buffer a pointer to data to write
		\return number of a bytes written or -1 on error
	*/
	virtual int write(int bytes_to_write, const char *buffer);
	//! Shut down the connection
	/*!
		Closes the connection gracefully.
	*/
	virtual void shutdown();
	//! Get the socket address
	/*!
		Obtains the connected socket ip address.
		
		\returns the socket ip address
	*/
	const std::string& address() const {
		return _address;
	}
	//! Get the socket port number
	/*!
		Obtains the connected socket port number.
		
		\returns the socket port number
	*/
	int port() const {
		return _port;
	}
protected:
	int socket_fd;
	std::string _address;
	int _port;
private:
	void set_blocked(bool state);
};

class tcp_socket: public socket {
public:
	//!	Constructor
	tcp_socket();
};

class udp_socket: public socket {
public:
	//!	Constructor
	udp_socket();
};

class unix_socket: public socket {
public:
	//!	Constructor
	unix_socket();
};

}

#endif /*_SOCKET_H_*/
