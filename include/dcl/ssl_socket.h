/*
 * ssl_socket.h
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

#ifndef _SSL_SOCKET_H_
#define _SSL_SOCKET_H_

#include <vector>
#include <string>

#include <dcl/socket.h>

namespace dbp {

class ssl_socket_impl;

//!	SSL Socket Class
/*!
	This class represents SSL-enabled network socket.
*/
class ssl_socket: public socket {
public:
	//!	Constructor
	ssl_socket();
	//! Constructor
	ssl_socket(const socket &src);
	//! Copy operator
	ssl_socket& operator=(const ssl_socket &src);
	//!	Destructor
	virtual ~ssl_socket();
	//! Get ssl_socket handle
	virtual int handle() const;
	//! Read data from the ssl_socket
	virtual int read(int bytes_to_read, char *buffer);
	//! Write data to the ssl_socket
	virtual int write(int bytes_to_write, const char *buffer);
	//! Shut down the connection
	virtual void shutdown();
	//! Load SSL certificates
	void load_certificates(
	  const std::string &ca,
	  const std::string &cert, const std::string &key,
	  const std::string &crl);
private:
	ssl_socket_impl *pimpl;
};

}

#endif /*_SSL_SOCKET_H_*/

