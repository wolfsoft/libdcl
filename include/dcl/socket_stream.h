/*
 * socket_stream.h
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

#ifndef _SOCKET_STREAM_H_
#define _SOCKET_STREAM_H_

#include <streambuf>

#include <dcl/noncopyable.h>
#include <dcl/socket.h>

namespace dbp {

//!	Class description.
/*!
	Detailed class description.
*/
class socket_stream: public std::streambuf, public noncopyable {
public:
	socket_stream(socket &s, std::size_t buf_size = 1500,
	  std::size_t put_back = 8);
protected:
	int_type underflow();
private:
	const std::size_t _put_back;
	std::vector<char> _buf;
	socket _s;
};

} // namespace

#endif /*_SOCKET_STREAM_H_*/
