/*
 * openssl_socket.cpp
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

#include <dcl/singleton.h>
#include <dcl/ssl_socket.h>
#include <dcl/strutils.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace dbp {

using namespace std;

class openssl_init: public singleton<openssl_init> {
	friend class singleton<openssl_init>;
public:
	virtual ~openssl_init() { }
private:
	openssl_init() {
		init();
	}
	openssl_init(const openssl_init&) {
		init();
	}
	void init() {
		SSL_library_init();
		SSL_load_error_strings();
	}
};

class ssl_socket_impl: public socket {
public:
	//! Constructor
	ssl_socket_impl(const socket &src): socket(src), ctx(NULL), ssl(NULL) {
		openssl_init::instance();
		// The method describes which SSL protocol we will be using
		// (SSLv2, SSLv3 and TLSv1)
		SSL_METHOD *method = const_cast<SSL_METHOD*>(SSLv23_server_method());
		// Create new context from method
		ctx = SSL_CTX_new(method);
		if (!ctx)
			throw socket_exception(_("can't initialize SSL context"));
		// Initialize SSL connection
		ssl = SSL_new(ctx);
		if (!ssl)
			throw socket_exception(_("can't initialize SSL context"));
		SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
		SSL_set_accept_state(ssl);
		SSL_set_fd(ssl, socket_fd);
	}
	~ssl_socket_impl() {
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	}
	//! Load certificates from files
	void load_certificates(
	  const std::string &ca,
	  const std::string &cert, const std::string &key,
	  const std::string &crl) {
	  	// Load CA certificate
		if (!ca.empty() && SSL_CTX_load_verify_locations(ctx, ca.c_str(), NULL))
			throw socket_exception(_("invalid CA certificate"));
		// TODO: load CRL file too
		// Load certificate file
		if (cert.empty() || (SSL_use_certificate_file(ssl,
		  cert.c_str(), SSL_FILETYPE_PEM) <= 0))
			throw socket_exception(_("can't load certificate file"));
		if (key.empty() || (SSL_use_PrivateKey_file(ssl, key.c_str(),
		  SSL_FILETYPE_PEM) <= 0))
			throw socket_exception(_("can't load private key file"));
		// Verify that the two keys goto together
		if (!SSL_check_private_key(ssl))
			throw socket_exception(_("private key is invalid"));
	}
	virtual int read(int bytes_to_read, char *buffer) {
		int nsize = SSL_read(ssl, buffer, bytes_to_read);
		if (nsize < 0 && SSL_get_error(ssl, nsize) == SSL_ERROR_WANT_READ)
			return socket::data_not_ready;
		if (nsize >= 0)
			return nsize;
		return socket::io_error;
	}
	virtual int write(int bytes_to_write, const char *buffer) {
		int nsize = SSL_write(ssl, buffer, bytes_to_write);
		if (nsize < 0 && SSL_get_error(ssl, nsize) == SSL_ERROR_WANT_WRITE)
			return socket::data_not_ready;
		if (nsize >= 0)
			return nsize;
		return socket::io_error;
	}
	virtual void shutdown() {
		SSL_shutdown(ssl);
	}
private:
	SSL_CTX *ctx;
	SSL *ssl;
};

} // namespace
