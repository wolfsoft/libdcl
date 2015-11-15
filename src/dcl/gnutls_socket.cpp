/*
 * gnutls_socket.cpp
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

#include <gcrypt.h>
#include <gnutls/gnutls.h>

namespace dbp {

using namespace std;

#define DH_BITS 1024

class gnutls_init: public singleton<gnutls_init> {
	friend class singleton<gnutls_init>;
public:
	virtual ~gnutls_init() {
		gnutls_global_deinit();
	}
private:
	gnutls_init() {
		init();
	}
	gnutls_init(const gnutls_init&) {
		init();
	}
	void init() {
		gcry_control(GCRYCTL_ENABLE_QUICK_RANDOM, 0);
		gnutls_global_init();
	}
};

class ssl_socket_impl: public socket {
public:
	//! Constructor
	ssl_socket_impl(const socket &src): socket(src), need_handshake(true),
	  x509_cred(NULL) {
		gnutls_init::instance();
		gnutls_dh_params_init(&dh_params);
		gnutls_dh_params_generate2(dh_params, DH_BITS);
		gnutls_priority_init(&priority_cache, "NORMAL", NULL);
		::gnutls_init(&session, GNUTLS_SERVER);
		gnutls_priority_set(session, priority_cache);
		gnutls_certificate_server_set_request(session, GNUTLS_CERT_REQUEST);
		gnutls_session_enable_compatibility_mode(session);
		if (socket_fd >= 0)
			gnutls_transport_set_ptr(session,
			  (gnutls_transport_ptr_t)socket_fd);
	}
	//!	Destructor
	virtual ~ssl_socket_impl() {
		if (x509_cred)
			gnutls_certificate_free_credentials(x509_cred);
		gnutls_deinit(session);
		gnutls_priority_deinit(priority_cache);
		gnutls_dh_params_deinit(dh_params);
	}
	//! Load certificates from files
	void load_certificates(
	  const std::string &ca,
	  const std::string &cert, const std::string &key,
	  const std::string &crl) {
		// allocate memory
		if (x509_cred)
			gnutls_certificate_free_credentials (x509_cred);
		gnutls_certificate_allocate_credentials(&x509_cred);
		// load certificates
		if (!ca.empty())
			gnutls_certificate_set_x509_trust_file(x509_cred, ca.c_str(),
			  GNUTLS_X509_FMT_PEM);
		if (!crl.empty())
			gnutls_certificate_set_x509_crl_file(x509_cred, crl.c_str(),
			  GNUTLS_X509_FMT_PEM);
		if (!cert.empty() && !key.empty())
			gnutls_certificate_set_x509_key_file(x509_cred, cert.c_str(),
			  key.c_str(), GNUTLS_X509_FMT_PEM);
		gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, x509_cred);
		// setup DH parameters
		gnutls_certificate_set_dh_params(x509_cred, dh_params);
	}
	virtual int read(int bytes_to_read, char *buffer) {
		if (need_handshake) {
			int ret = gnutls_handshake(session);
			if (ret < 0)
				return gnutls_error_is_fatal(ret) ? socket::io_error :
				  socket::data_not_ready;
			need_handshake = false;
		}
		int nsize = gnutls_record_recv(session, buffer, bytes_to_read);
		if (nsize == GNUTLS_E_AGAIN || nsize == GNUTLS_E_INTERRUPTED)
			return socket::data_not_ready;
		if (nsize >= 0)
			return nsize;
		return socket::io_error;
	}
	virtual int write(int bytes_to_write, const char *buffer) {
		if (need_handshake) {
			int ret = gnutls_handshake(session);
			if (ret < 0)
				return gnutls_error_is_fatal(ret) ? socket::io_error :
				  socket::data_not_ready;
			need_handshake = false;
		}
		int nsize = gnutls_record_send(session, buffer, bytes_to_write);
		if (nsize == GNUTLS_E_AGAIN || nsize == GNUTLS_E_INTERRUPTED)
			return socket::data_not_ready;
		if (nsize >= 0)
			return nsize;
		return socket::io_error;
	}
	virtual void shutdown() {
		gnutls_bye(session, GNUTLS_SHUT_WR);
	}
private:
	bool need_handshake;
	gnutls_certificate_credentials_t x509_cred;
	gnutls_session_t session;
	gnutls_priority_t priority_cache;
	gnutls_dh_params_t dh_params;
};

} // namespace
