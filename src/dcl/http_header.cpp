/*
 * http_header.cpp
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2010 Dennis Prochko <wolfsoft@mail.ru>
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

#include <map>

#include "dcl/http_header.h"
#include "dcl/strutils.h"

namespace dbp {

using namespace std;

// http_error

namespace http_error {

class convert {
public:
	std::string operator()(http_error value) {
		typedef pair<http_error, const char*> status_map;
		status_map const table[] = {
			status_map(http_error::error_continue, "Continue"),
			status_map(http_error::switching_protocols, "Switching Protocols"),
			status_map(http_error::ok, "OK"),
			status_map(http_error::created, "Created"),
			status_map(http_error::accepted, "Accepted"),
			status_map(http_error::non_authoritative_information,
			  "Non Authoritative Information"),
			status_map(http_error::no_content, "No Content"),
			status_map(http_error::reset_content, "Reset Content"),
			status_map(http_error::partial_content, "Partial Content"),
			status_map(http_error::multiple_choices, "Multiple Choices"),
			status_map(http_error::moved_permanently, "Moved Permanently"),
			status_map(http_error::found, "Found"),
			status_map(http_error::see_other, "See Other"),
			status_map(http_error::not_modified, "Not Modified"),
			status_map(http_error::use_proxy, "Use Proxy"),
			status_map(http_error::temporary_redirect, "Temporary Redirect"),
			status_map(http_error::bad_request, "Bad Request"),
			status_map(http_error::unauthorized, "Unauthorized"),
			status_map(http_error::payment_required, "Payment Required"),
			status_map(http_error::forbidden, "Forbidden"),
			status_map(http_error::not_found, "Not Found"),
			status_map(http_error::method_not_allowed, "Method Not Allowed"),
			status_map(http_error::not_acceptable, "Not Acceptable"),
			status_map(http_error::proxy_authentication_required,
			  "Proxy Authentication Required"),
			status_map(http_error::request_timeout, "Request Timeout"),
			status_map(http_error::conflict, "Conflict"),
			status_map(http_error::gone, "Gone"),
			status_map(http_error::length_required, "Length Required"),
			status_map(http_error::precondition_failed, "Precondition Failed"),
			status_map(http_error::request_entity_too_large,
			  "Request Entity Too Large"),
			status_map(http_error::request_uri_too_long, "Request-URI Too Long"),
			status_map(http_error::unsupported_media_type,
			  "Unsupported Media Type"),
			status_map(http_error::requested_range_not_satisfiable,
			  "Requested Range Not Satisfiable"),
			status_map(http_error::expectation_failed,
			  "Expectation Failed"),
			status_map(http_error::internal_server_error, "Internal Server Error"),
			status_map(http_error::not_implemented, "Not Implemented"),
			status_map(http_error::bad_gateway, "Bad Gateway"),
			status_map(http_error::service_unavailable, "Service Unavailable"),
			status_map(http_error::gateway_timeout, "Gateway Timeout"),
			status_map(http_error::http_version_not_supported,
			  "HTTP Version Not Supported")
		};
		map<http_error, const char*> _map(table,
		  table + sizeof(table) / sizeof(table[0]));
		return _map[value];
	}
};

} // namespace

// http_method

namespace http_method {

class convert {
public:
	http_method operator()(const std::string &value) const {
		typedef pair<std::string, http_method> status_map;
		status_map const table[] = {
			status_map("GET", http_method::get),
			status_map("PUT", http_method::put),
			status_map("POST", http_method::post),
			status_map("DELETE", http_method::del),
			status_map("CONNECT", http_method::connect),
			status_map("OPTIONS", http_method::options),
			status_map("TRACE", http_method::trace),
			status_map("PATCH", http_method::patch),
			status_map("PROPFIND", http_method::propfind),
			status_map("PROPPATCH", http_method::proppatch),
			status_map("MKCOL", http_method::mkcol),
			status_map("COPY", http_method::copy),
			status_map("MOVE", http_method::move),
			status_map("LOCK", http_method::lock),
			status_map("UNLOCK", http_method::unlock),
			status_map("VERSION_CONTROL", http_method::version_control),
			status_map("CHECKOUT", http_method::checkout),
			status_map("UNCHECKOUT", http_method::uncheckout),
			status_map("CHECKIN", http_method::checkin),
			status_map("UPDATE", http_method::update),
			status_map("LABEL", http_method::label),
			status_map("REPORT", http_method::report),
			status_map("MKWORKSPACE", http_method::mkworkspace),
			status_map("MKACTIVITY", http_method::mkactivity),
			status_map("BASELINE_CONTROL", http_method::baseline_control),
			status_map("MERGE", http_method::merge),
			status_map("HEAD", http_method::head)
		};
		map<std::string, http_method> _map(table,
		  table + sizeof(table) / sizeof(table[0]));
		if (_map.find(value) != _map.end())
			return _map[value];
		else
			return http_method::unknown;
	};
	std::string operator()(const http_method value) const {
	typedef pair<http_method, const char*> status_map;
		status_map const table[] = {
			status_map(http_method::get, "GET"),
			status_map(http_method::put, "PUT"),
			status_map(http_method::post, "POST"),
			status_map(http_method::del, "DELETE"),
			status_map(http_method::connect, "CONNECT"),
			status_map(http_method::options, "OPTIONS"),
			status_map(http_method::trace, "TRACE"),
			status_map(http_method::patch, "PATCH"),
			status_map(http_method::propfind, "PROPFIND"),
			status_map(http_method::proppatch, "PROPPATCH"),
			status_map(http_method::mkcol, "MKCOL"),
			status_map(http_method::copy, "COPY"),
			status_map(http_method::move, "MOVE"),
			status_map(http_method::lock, "LOCK"),
			status_map(http_method::unlock, "UNLOCK"),
			status_map(http_method::version_control, "VERSION_CONTROL"),
			status_map(http_method::checkout, "CHECKOUT"),
			status_map(http_method::uncheckout, "UNCHECKOUT"),
			status_map(http_method::checkin, "CHECKIN"),
			status_map(http_method::update, "UPDATE"),
			status_map(http_method::label, "LABEL"),
			status_map(http_method::report, "REPORT"),
			status_map(http_method::mkworkspace, "MKWORKSPACE"),
			status_map(http_method::mkactivity, "MKACTIVITY"),
			status_map(http_method::baseline_control, "BASELINE_CONTROL"),
			status_map(http_method::merge, "MERGE"),
			status_map(http_method::head, "HEAD")
		};
		map<http_method, const char*> _map(table,
		  table + sizeof(table) / sizeof(table[0]));
		if (value == http_method::unknown)
			return "";
		else
			return _map[value];
	};
};

} // namespace

// friend IO operators

istream& operator>>(istream &in, http_header &h) {
	string s;
	in >> s;
	h.set_header(s);
	return in;
}

ostream& operator<<(ostream &out, const http_request &h) {
	// Output headers first
	{
		http_header::http_headers::const_iterator i = h.headers.begin();
		while (i != h.headers.end()) {
			out << i->first + ": " + i->second << CRLF;
			++i;
		};
	}
	// Output cookies
	{
		string s;
		http_cookies::const_iterator i = h.cookies.begin();
		while (i != h.cookies.end()) {
			s += i->name + "=" + i->value;
			++i;
			if (i != h.cookies.end())
				s += "; ";
		}
		if (!s.empty())
			out << "Cookie: " << s << CRLF;
	}
	// Output data
	out << CRLF;
	if (h.get_content_size() > 0)
		out.write(h.get_content(), h.get_content_size());
	return out;
}

ostream& operator<<(ostream &out, const http_response &h) {
	// Output headers first
	http_header::http_headers::const_iterator i = h.headers.begin();
	while (i != h.headers.end()) {
		out << i->first + ": " + i->second << CRLF;
		++i;
	};
	// Output cookies
	for (http_cookies::const_iterator i = h.cookies.begin();
	  i != h.cookies.end(); ++i) {
		out << "Set-Cookie: " << i->str() << CRLF;
	}
	// Output data
	out << CRLF;
	if (h.get_content_size() > 0)
		out.write(h.get_content(), h.get_content_size());
	return out;
}

// http_cookie

http_cookie::http_cookie(const std::string &cookie): secure(false), http_only(false) {
	strings fields = tokenize()(cookie, ";");
	for (strings::const_iterator i = fields.begin(); i != fields.end(); ++i) {
		string p, v;
		tokenize()(*i, p, v, false, "=");
		p = trim()(p);
		v = trim()(v);
		if (p == string("expires"))
			expires = v;
		else
		if (p == string("domain"))
			domain = v;
		else
		if (p == string("path"))
			path = v;
		else
		if (p == string("secure"))
			secure = true;
		else
		if (p == string("HttpOnly"))
			http_only = true;
		else {
			name = p;
			value = v;
		}
	}
}

std::string http_cookie::str() const {
	string s;
	if (!name.empty()) {
		s += name + "=" + value;
		if (expires)
			s += "; expires=" + expires.as_gmt();
		if (!domain.empty())
			s += "; domain=" + domain;
		if (!path.empty())
			s += "; path=" + path;
		if (secure)
			s += "; secure";
		if (http_only)
			s += "; HttpOnly";
	}
	return s;
}

// http_header

void http_header::set_header(const std::string &key, const std::string &value) {
	if (compare()(key, "Cookie")) {
		strings c = tokenize()(value, ";");
		for (strings::const_iterator i = c.begin(); i != c.end(); ++i)
			cookies.push_back(http_cookie(*i));
	} else
	if (compare()(key, "Set-Cookie"))
		cookies.push_back(http_cookie(value));
	else
		headers[key] = value;
}

void http_header::set_header(const std::string &header) {
	size_t pos = header.find_first_of(':');
	if (pos != string::npos) {
		string p = trim()(header.substr(0, pos));
		string v = trim()(header.substr(pos + 1));
		set_header(p, v);
	}
}

const std::string& http_header::get_header(const std::string &key) const {
	http_headers::const_iterator i = headers.find(key);
	if (i == headers.end()) {
		return _empty_string;
	} else {
		return i->second;
	}
}

void http_header::set_content(const std::string &value) {
	set_content(value.length(), value.c_str());
}

void http_header::set_content(int value_size, const char *value) {
	if (value_size > 0) {
		content.reserve(value_size + 1);
		content.insert(content.begin(), value, value + value_size);
	}
	content.push_back(0);
	headers["Content-Length"] = to_string<int>(value_size);
}

void http_header::add_content(int value_size, std::istream &value) {
	size_t oldsize = content.size();
	if (oldsize == 0) {
		content.push_back(0);
		oldsize = 1;
	}
	if (value_size > 0) {
		content.resize(oldsize + value_size);
		value.read(&content[oldsize - 1], value_size);
		content[oldsize + value_size] = 0;
		headers["Content-Length"] = to_string<int>(oldsize + value_size - 1);
	} else {
		headers["Content-Length"] = "0";
	}
}

void http_header::add_content(std::istream &value) {
	streampos pos = value.tellg();
	value.seekg(0, ios_base::end);
	size_t size = value.tellg() - pos;
	value.seekg(pos);
	if (size == 0) {
		value.setstate(ios::eofbit);
	} else {
		add_content(size, value);
	}
}

const http_cookies& http_header::get_cookies() const {
	return cookies;
}

void http_header::set_cookies(const http_cookies &c) {
	cookies.resize(c.size());
	copy(c.begin(), c.end(), cookies.begin());
}

void http_header::set_cookie(const http_cookie &c) {
	cookies.clear();
	cookies.push_back(c);
}

void http_header::add_cookie(const http_cookie &c) {
	cookies.push_back(c);
}

// http_request

const std::string http_request::method() const {
	return http_method::convert()(_method);
}

void http_request::set_method(const std::string &method) {
	_method = http_method::convert()(method);
}

// http_response

http_method::http_method http_response::get_allow() {
	if (headers.find("Allow") == headers.end())
		return http_method::unknown;
	else
		return http_method::convert()(headers["Allow"]);
}

void http_response::set_allow(http_method::http_method method) {
	headers["Allow"] = http_method::convert()(method);
}

std::string http_response::get_status() const {
	return to_string<int>(_status) + " " + http_error::convert()(_status);
}

} // namespace

