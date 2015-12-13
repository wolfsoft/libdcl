/*
 * http_header.h
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

#ifndef _HTTP_HEADER_H_
#define _HTTP_HEADER_H_

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <dcl/datetime.h>
#include <dcl/strutils.h>

namespace dbp {

#ifndef CRLF
#define CRLF "\r\n"
#endif

//!	HTTP error code
/*!
	This enum contains the standartized HTTP error codes as described in
	RFC 2616.
*/
namespace http_error {
	enum http_error {
		error_continue = 100,
		switching_protocols = 101,
		ok = 200,
		created = 201,
		accepted = 202,
		non_authoritative_information = 203,
		no_content = 204,
		reset_content = 205,
		partial_content = 206,
		multiple_choices = 300,
		moved_permanently = 301,
		found = 302,
		see_other = 303,
		not_modified = 304,
		use_proxy = 305,
		temporary_redirect = 307,
		bad_request = 400,
		unauthorized = 401,
		payment_required = 402,
		forbidden = 403,
		not_found = 404,
		method_not_allowed = 405,
		not_acceptable = 406,
		proxy_authentication_required = 407,
		request_timeout = 408,
		conflict = 409,
		gone = 410,
		length_required = 411,
		precondition_failed = 412,
		request_entity_too_large = 413,
		request_uri_too_long = 414,
		unsupported_media_type = 415,
		requested_range_not_satisfiable = 416,
		expectation_failed = 417,
		internal_server_error = 500,
		not_implemented = 501,
		bad_gateway = 502,
		service_unavailable = 503,
		gateway_timeout = 504,
		http_version_not_supported = 505
	};
}

//!	HTTP method type
/*!
	This enum contains the HTTP method types as described in RFC 2616.
*/
namespace http_method {
	enum http_method {
		get = 0,
		put = 1,
		post = 2,
		del = 3,
		connect = 4,
		options = 5,
		trace = 6,
		patch = 7,
		propfind = 8,
		proppatch = 9,
		mkcol = 10,
		copy = 11,
		move = 12,
		lock = 13,
		unlock = 14,
		version_control = 15,
		checkout = 16,
		uncheckout = 17,
		checkin = 18,
		update = 19,
		label = 20,
		report = 21,
		mkworkspace = 22,
		mkactivity = 23,
		baseline_control = 24,
		merge = 25,
		unknown = 26,
		head = 27
	};
}

//!	HTTP cookie
/*!
	This class represents http cookie (RFC 2109).
*/
class http_cookie {
public:
	bool secure;
	bool http_only;
	std::string name;
	std::string value;
	std::string domain;
	datetime expires;
	std::string path;
	//! Constructor
	http_cookie(): secure(false), http_only(false) { };
	//! Constructor
	http_cookie(const std::string &name, const std::string &value):
	  secure(false), http_only(false), name(name), value(value) { };
	//! Constructor
	/*!
		Parse the string containing cookie.

		\param cookie the string to parse.
	*/
	http_cookie(const std::string &cookie);
	//! Get string representation
	std::string str() const;
};

typedef std::vector<http_cookie> http_cookies;

//!	HTTP header
/*!
	This class represents http headers.
*/
class http_header {
   	//!	Set content from a stream
	friend std::istream& operator>>(std::istream&, http_header&);
   	//!	Get a stream from content
	friend std::ostream& operator<<(std::ostream&, const http_header&);
public:

	class ci_less: std::binary_function<std::string, std::string, bool> {
	public:

		class nocase_compare : public std::binary_function<unsigned char,unsigned char,bool> {
		public:
			bool operator() (const unsigned char& c1, const unsigned char& c2) const {
				return tolower (c1) < tolower (c2); 
			}
		};

		bool operator() (const std::string & s1, const std::string & s2) const {
			return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), nocase_compare());
		}
	};

	//! HTTP header and value pair collection
	typedef std::map<std::string, std::string, ci_less> http_headers;
	//! Constructor
	http_header(): _http_version("1.1") { }
	//! Initialize a header
	/*!
		Set up a header and initialize it with value.
		
		\param key a header to be initialized.
		\param value a header value.
	*/
	void set_header(const std::string &key, const std::string &value);
	//! Initialize a header
	/*!
		Parse a given string (specified by a standard web header, where header
		and its value are divided by a semicolon). Set up a header and
		initialize it with parsed value.
		
		\param header a "header: value" pair.
	*/
	void set_header(const std::string &header);
	//! Get the header value
	/*!
		Get a header value specified by key.
		
		\param key a value key.
		\return a header value.
	*/
	const std::string& get_header(const std::string &key) const;
	//! Get iterator to the all headers
	/*!
		Obtaining a const_iterator to the all headers added before.
		
		\return a reference to the collection of http headers.
	*/
	const http_headers& all_headers() const {
		return headers;
	}
	//! Initialize the content
	/*!
		\param value a string to set as content.
	*/
	void set_content(const std::string &value);
	//! Initialize the content
	/*!
		\param value_size a string length.
		\param value a C string to set as content.
	*/
	void set_content(int value_size, const char *value);
	//! Append the content
	/*!
		Add content from the source stream.

		\param value_size the content size.
		\param value a stream to set content from.
	*/
	void add_content(int value_size, std::istream &value);
	//! Append the content
	/*!
		Detect size of the source stream and add whole content
		from the source to the content buffer.

		\param value a stream to set content from.
	*/
	void add_content(std::istream &value);
	//!	Get content
	/*!
		\return a pointer to the content.
	*/
	const char* get_content() const { return &content[0]; }
	//!	Get content size
	/*!
		\return content length in bytes
	*/
	int get_content_size() const {
	  return content.empty() ? 0 : content.size() - 1;
	}
	//! Get content type
	/*!
		\return MIME content type
	*/
	const std::string get_content_type() const {
		http_headers::const_iterator i = headers.find("Content-Type");
		if (i == headers.end()) {
			return "text/plain";
		} else {
			return i->second;
		}
	}
	//! Set content type
	/*!
		\param value the MIME content type
	*/
	void set_content_type (const std::string &value) {
		headers["Content-Type"] = value;
	}
	//! Get HTTP version number
	const std::string& http_version() const {
		return _http_version;
	}
	//! Set HTTP version number
	http_header& http_version(const std::string &value) {
		_http_version = value;
		return *this;
	}
	//! Return cookies
	const http_cookies& get_cookies() const;
	//! Set cookies
	void set_cookies(const http_cookies&);
	//! Clear cookies and set single cookie
	void set_cookie(const http_cookie&);
	//! Add the cookie
	void add_cookie(const http_cookie&);
protected:
	http_headers headers;
	http_cookies cookies;
private:
	typedef std::vector<char> buffer;
	buffer content;
	std::string _empty_string;
	std::string _http_version;
};


//!	HTTP request
/*!
	This class represents a HTTP request.
*/
class http_request: public http_header {
	friend std::istream& operator>>(std::istream&, http_header&);
	friend std::ostream& operator<<(std::ostream&, const http_request&);
public:
	const std::string& get_auth_type() const {
		return _auth_type;
	};
	void set_auth_type(const std::string &value) {
		_auth_type = value;
	};
	const std::string& get_gateway_interface() const {
		return _gateway_interface;
	};
	void set_gateway_interface(const std::string &value) {
		_gateway_interface = value;
	};
	const std::string& get_path_info() const {
		return _path_info;
	};
	void set_path_info(const std::string &value) {
		_path_info = value;
	};
	const std::string& get_path_translated() const {
		return _path_translated;
	};
	void set_path_translated(const std::string &value) {
		_path_translated = value;
	};
	const std::string& get_query_string() const {
		return _query_string;
	};
	void set_query_string(const std::string &value) {
		_query_string = value;
	};
	const std::string& get_remote_addr() const {
		return _remote_addr;
	};
	void set_remote_addr(const std::string &value) {
		_remote_addr = value;
	};
	const std::string& get_remote_host() const {
		return _remote_host;
	};
	void set_remote_host(const std::string &value) {
		_remote_host = value;
	};
	const std::string& get_remote_ident() const {
		return _remote_ident;
	};
	void set_remote_ident(const std::string &value) {
		_remote_ident = value;
	};
	const std::string& get_remote_user() const {
		return _remote_user;
	};
	void set_remote_user(const std::string &value) {
		_remote_user = value;
	};
	const std::string& get_script_name() const {
		return _script_name;
	};
	void set_script_name(const std::string &value) {
		_script_name = value;
	};
	const std::string& get_server_name() const {
		return _server_name;
	};
	void set_server_name(const std::string &value) {
		_server_name = value;
	};
	int get_server_port() const {
		return _server_port;
	};
	void set_server_port(int value) {
		_server_port = value;
	};
	const std::string& get_server_protocol() const {
		return _server_protocol;
	};
	void set_server_protocol(const std::string &value) {
		_server_protocol = value;
	};
	http_method::http_method get_method() const {
		return _method;
	};
	const std::string method() const;
	void set_method(http_method::http_method method) {
		_method = method;
	};
	void set_method(const std::string &method);
	const std::string& get_server_software() const {
		return _server_software;
	}
	void set_server_software(const std::string &value) {
		_server_software = value;
	};
	const std::string& get_host() const {
		return get_header("Host");
	}
	void set_host(const std::string &value) {
		headers["Host"] = value;
	}
	void set_https(bool use_https) {
		_https = use_https;
	}
	bool get_https() const {
		return _https;
	}
private:
	std::string _auth_type;
	std::string _gateway_interface;
	std::string _path_info;
	std::string _path_translated;
	std::string _query_string;
	std::string _remote_addr;
	std::string _remote_host;
	std::string _remote_ident;
	std::string _remote_user;
	std::string _script_name;
	std::string _server_name;
	int _server_port;
	std::string _server_protocol;
	http_method::http_method _method;
	std::string _server_software;
	bool _https;
};

//!	HTTP response
/*!
	This class represents a HTTP response.
*/
class http_response: public http_header {
	friend std::istream& operator>>(std::istream&, http_header&);
	friend std::ostream& operator<<(std::ostream&, const http_response&);
public:
	http_response() {
		set_status(http_error::ok);
		set_content_type("text/plain; charset=utf-8");
	};
	http_method::http_method get_allow();
	void set_allow(http_method::http_method method);
	//! Get/Set individual headers
	std::string get_status() const;
	http_error::http_error get_status_code() const {
		return _status;
	};
	void set_status(http_error::http_error code) {
		_status = code;
	};
	const std::string& get_connection() {
		return get_header("Connection");
	};
	const std::string& get_transfer_encoding() const {
		return get_header("Transfer-Encoding");
	};
private:
	http_error::http_error _status;
};

}

#endif /*_HTTP_HEADER_H_*/
