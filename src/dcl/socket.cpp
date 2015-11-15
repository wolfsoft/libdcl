/*
 * socket.cpp
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

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <win32/socket_impl.cpp>
#else
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#endif

#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <dcl/socket.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

#ifdef _WIN32
#ifndef HAVE_INET_NTOP
const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
	if (af == AF_INET) {
		struct sockaddr_in in;
		memset(&in, 0, sizeof(in));
		in.sin_family = AF_INET;
		memcpy(&in.sin_addr, src, sizeof(struct in_addr));
		getnameinfo((struct sockaddr *)&in,
		  sizeof(struct sockaddr_in), dst, cnt, NULL, 0, NI_NUMERICHOST);
		return dst;
	} else
		if (af == AF_INET6) {
			struct sockaddr_in6 in;
			memset(&in, 0, sizeof(in));
			in.sin6_family = AF_INET6;
			memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
			getnameinfo((struct sockaddr *)&in,
			  sizeof(struct sockaddr_in6), dst, cnt, NULL, 0, NI_NUMERICHOST);
			return dst;
	}
	return NULL;
}
#endif

#ifndef HAVE_INET_PTON
int inet_pton(int af, const char *src, void *dst) {
	struct addrinfo hints, *res = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_NUMERICHOST;
	hints.ai_family = af;
	if (getaddrinfo(src, NULL, &hints, &res) != 0) {
		return -1;
	}
	if (res) {
		memcpy(dst, &((struct sockaddr_in *)(res->ai_addr))->sin_addr,
		  sizeof(struct in_addr));
		freeaddrinfo(res);
		return 1;
	}
	return 0;
}
#endif
#endif

socket::socket(): socket_fd(-1), _port(0) {
#ifdef _WIN32
	local::winsock2_init::instance();
#endif
}

void socket_address::parse(const std::string &addr) {
	// split a socket address to a host and port pair
	vector<string> a = tokenize()(addr, ":");
	// resolve host name to a IP address
	host = a[0];
	if (!host.empty() && (host != "*")) {
		hostent *h;
		if ((h = gethostbyname(host.c_str())) == NULL) {
			switch (h_errno) {
				case HOST_NOT_FOUND:
					throw socket_exception(
					  (format(_("host '{0}' is not found")) % host).str());
					break;
				case NO_DATA:
					throw socket_exception(
					  (format(_("can't access host '{0}'")) % host).str());
				default:
					throw socket_exception(
					  (format(_("can't resolve host '{0}'")) % host).str());
			}
					
		}
		// if several IP addresses are assigned to a host, add them all
		char str[INET6_ADDRSTRLEN];
		for (char **pptr = h->h_addr_list; *pptr != NULL; pptr++)
			push_back(inet_ntop(h->h_addrtype, *pptr, str, sizeof(str)));
	}
	// if a port part exists, convert it to integer
	if (a[1].empty())
		port = 0;
	else
		port = from_string<int>(a[1]);
}

socket::~socket() {
	if (socket_fd >= 0) {
		// block the socket
		set_blocked(true);
		::close(socket_fd);
		socket_fd = -1;
	}
}

bool socket::connect(const string &address, int port) {
	struct sockaddr_in s;
	// Convert given addresses to desired format
	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_port = htons(port);
	if (inet_pton(AF_INET, address.c_str(), &s.sin_addr) <= 0)
		throw socket_exception(_("can't assign an address to a socket"));
	// try to connect
	if (::connect(socket_fd, (struct sockaddr*)&s, sizeof(s)) < 0) {
		socket_fd = -1;
		return false;
	}
	_address = address;
	_port = port;
	return true;
}

void socket::bind(const socket_address &address) {
	struct sockaddr_in s;
	// Convert given addresses to desired format
	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	if (address.empty())
		s.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		if (inet_pton(AF_INET, address[0].c_str(), &s.sin_addr) <= 0)
			throw socket_exception(_("can't assign an address to a socket"));
	s.sin_port = htons(address.port);
	if (!address.empty())
		_address = address[0];
	else
		_address.clear();
	_port = address.port;
	// setup socket options
	int reuseaddr = 1;
	if (::setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
#ifdef _WIN32
	  (const char*)&reuseaddr,
#else
	  (const void*)&reuseaddr,
#endif
	  sizeof(reuseaddr)) < 0)
		throw socket_exception(_("can't setup socket options"));
	// set non-blocking
	set_blocked(false);
	// try to bind
	if (::bind(socket_fd, (struct sockaddr*)&s, sizeof(s)) < 0) {
		socket_fd = -1;
		throw socket_exception(_("can't bind to the address or port"));
	}
}

void socket::listen() {
	if (::listen(socket_fd, -1) < 0)
		throw socket_exception(_("can't listen socket"));
}

socket socket::accept() {
	socket rslt;
	struct sockaddr_in s;
	socklen_t sockaddr_in_size = sizeof(s);
	memset(&s, 0, sockaddr_in_size);
	int client_socket_fd;
	// accept the connection
	if ((client_socket_fd = ::accept(socket_fd,
	  (struct sockaddr*)&s, &sockaddr_in_size)) < 0)
		throw socket_exception(_("can't accept incoming connection"));
	// initialize result
	rslt._address = inet_ntoa(s.sin_addr);
	rslt._port = ntohs(s.sin_port);
	rslt.socket_fd = client_socket_fd;
	// set non-blocking
	rslt.set_blocked(false);
	return rslt;
}

void socket::shutdown() {
#ifdef _WIN32
	::shutdown(socket_fd, SD_BOTH);
#else
	::shutdown(socket_fd, SHUT_RDWR);
#endif
}

int socket::read(int bytes_to_read, char *buffer) {
	int nread;
	if ((nread = ::recv(socket_fd, buffer, bytes_to_read, 0)) < 0) {
#ifdef _WIN32
		if (WSAGetLastError() == EAGAIN)
#else
		if (errno == EAGAIN)
#endif
			nread = data_not_ready;
		else
			return io_error;
	}
	return nread;
}

int socket::write(int bytes_to_write, const char *buffer) {
	int nwritten;
	if ((nwritten = ::send(socket_fd, buffer, bytes_to_write, 0)) < 0) {
#ifdef _WIN32
		if (WSAGetLastError() == EAGAIN)
#else
		if (errno == EAGAIN)
#endif
			nwritten = data_not_ready;
		else
			return io_error;
	}
	return nwritten;
}

void socket::set_blocked(bool state) {
#ifdef _WIN32
	u_long opts = state;
	if (ioctlsocket(socket_fd, FIONBIO, &opts) < 0)
		throw socket_exception(_("can't change socket blocking state"));
#else
	int opts = fcntl(socket_fd, F_GETFL);
	if (opts < 0)
		throw socket_exception(_("can't get socket blocking state"));
	if (state)
		opts = (opts & O_NONBLOCK);
	else
		opts = (opts | O_NONBLOCK);
	if (fcntl(socket_fd, F_SETFL, opts) < 0)
		throw socket_exception(_("can't change socket blocking state"));
#endif
}

tcp_socket::tcp_socket(): socket() {
	socket_fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd < 0)
		throw socket_exception(_("can't create socket"));
}

udp_socket::udp_socket(): socket() {
	socket_fd = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket_fd < 0)
		throw socket_exception(_("can't create socket"));
}

unix_socket::unix_socket(): socket() {
#ifdef _WIN32
	socket_fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
	socket_fd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
#endif
	if (socket_fd < 0)
		throw socket_exception(_("can't create socket"));
}

} // namespace
