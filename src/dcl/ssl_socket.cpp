/*
 * ssl_socket.cpp
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

#include <dcl/ssl_socket.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_OPENSSL
#include "openssl_socket.cpp"
#else
#ifdef HAVE_GNUTLS
#include "gnutls_socket.cpp"
#endif
#endif

namespace dbp {

using namespace std;

ssl_socket::ssl_socket() {
	pimpl = new ssl_socket_impl(*this);
}

ssl_socket::ssl_socket(const socket &src): socket(src) {
	pimpl = new ssl_socket_impl(*this);
}

ssl_socket& ssl_socket::operator=(const ssl_socket &src) {
    delete pimpl;
	pimpl = new ssl_socket_impl(*src.pimpl);
	return *this;
}

ssl_socket::~ssl_socket() {
	delete pimpl;
}

int ssl_socket::handle() const {
	return pimpl->handle();
}

void ssl_socket::load_certificates(
  const std::string &ca,
  const std::string &cert, const std::string &key,
  const std::string &crl) {
	pimpl->load_certificates(ca, cert, key, crl);
}

int ssl_socket::read(int bytes_to_read, char *buffer) {
	return pimpl->read(bytes_to_read, buffer);
}

int ssl_socket::write(int bytes_to_write, const char *buffer) {
	return pimpl->write(bytes_to_write, buffer);
}

void ssl_socket::shutdown() {
	pimpl->shutdown();
}

} // namespace
