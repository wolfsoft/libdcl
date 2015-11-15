/*
 * socket_stream.cpp
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

#include <cstring>

#include <dcl/socket_stream.h>

namespace dbp {

using namespace std;

socket_stream::socket_stream(socket &s, size_t buf_size, size_t put_back):
  _put_back(max(put_back, size_t(1))),
  _buf(max(buf_size, _put_back) + _put_back), _s(s) {
	char *end = &_buf.front() + _buf.size();
	setg(end, end, end);
}

std::streambuf::int_type socket_stream::underflow() {
	// is buffer not exhausted?
	if (gptr() < egptr())
		return traits_type::to_int_type(*gptr());
	char *base = &_buf.front();
	char *start = base;
	if (eback() == base) {
		// Make arrangements for putback characters
		// when this isn't the first fill
		memmove(base, egptr() - _put_back, _put_back);
		start += _put_back;
	}
	// start is now the start of the buffer, proper
	// read from socket into the provided buffer
/*	size_t n = std::fread(start, 1, _buf.size() - (start - base), fptr_);
    if (n == 0)
        return traits_type::eof();
    // Set buffer pointers
    setg(base, start, start + n);
*/
    return traits_type::to_int_type(*gptr());
}

} // namespace
