/*
 * url.cpp
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

#include <sstream>
#include <iomanip>
#include <stdlib.h>

#ifdef WIN32
#include "win32/socket_impl.cpp"
#else
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <dcl/url.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

url::url(): port(0) {
#ifdef WIN32
	local::winsock2_init::instance();
#endif
}

url::url(const std::string &value): port(0) {
#ifdef WIN32
	local::winsock2_init::instance();
#endif
	parse(value);
}

url::url(const char *value): port(0) {
#ifdef WIN32
		local::winsock2_init::instance();
#endif
	if (value) parse(value);
}

void url::parse(const string &value) {
	string t(value);
	string::size_type pos;
#ifdef WIN32
	// Replace all '\' path windowsisms to RFC compatible '/' ones.
	pos = t.find('\\');
	while (pos != string::npos) {
		t.replace(pos, 1, "/");
		pos = t.find('\\');
	}
#endif
	// search for scheme
	pos = t.find(':');
	if ((pos != string::npos) &&
	  // all known schemes have length above 3; windows drive letters is 1
	  // symbol long. So, we check it to difference scheme from windows path
	  (pos > 2) &&
	  (t.length() > pos + 2) &&
	  (t[pos + 1] == '/') &&
	  (t[pos + 2] == '/')) {
		scheme = t.substr(0, pos);
		t.erase(0, pos + 3);
	} else {
		scheme = "";
	}
	if (!scheme.empty()) {
		// search for ip_server
		if ((pos = t.find('/')) == string::npos) {
			host = t;
			t.clear();
		} else {
			host = t.substr(0, pos);
			t.erase(0, pos);
		}
	}
	if (!scheme.empty() && (scheme != "file")) {
		// assign a port by a scheme
		struct servent *s;
		if ((s = getservbyname(scheme.c_str(), "tcp")) != NULL) {
			port = ntohs(s->s_port);
		}
		// search for username
		if ((pos = host.find('@')) != string::npos) {
			user = host.substr(0, pos);
			host.erase(0, pos + 1);
			// search for password
			if ((pos = user.find(':')) != string::npos) {
				string s = user.substr(0, pos);
				user.erase(0, pos + 1);
				password = user;
				user = s;
			}
		}
		// search for port number
		if ((pos = host.find(':')) != string::npos) {
			string s = host.substr(0, pos);
			host.erase(0, pos + 1);
			if (host.length() > 0)
				port = from_string<int>(host);
			host = s;
		}
		// search for parameters
		if ((pos = t.find(';')) != string::npos) {
			parameters = t.substr(pos + 1);
		    t.erase(pos);
		}
		// search for fragment
		if ((pos = t.find('#')) != string::npos) {
			fragment = t.substr(pos + 1);
		    t.erase(pos);
		}
		// search for query
		if ((pos = t.find('?')) != string::npos) {
			query = t.substr(pos + 1);
		    t.erase(pos);
		}
	}
	// remaining string is a path
	if (t.empty() || t[0] != '/')
		path = "/" + t;
	else
		path = t;
}


std::string url::get_full_path() const {
	string rslt = "";
	if (!path.empty()) {
		rslt += path;
	}
	if (!query.empty()) {
		rslt += "?" + query;
	}
	if (!fragment.empty()) {
		rslt += "#" + fragment;
	}
	if (!parameters.empty()) {
		rslt += ";" + parameters;
	}
	return rslt;
}

void url::copy_from(const url &u) {
	if (!u.scheme.empty())
		scheme = u.scheme;
	if (!u.user.empty())
		user = u.user;		
	if (!u.password.empty())
		password = u.password;
	if (!u.host.empty()) {
		host = u.host;
	}
	if (u.port > 0) {
		port = u.port;
	}
	if (!u.path.empty()) {
		path = u.path;
	}
	if (!u.query.empty()) {
		query = u.query;
	}
	if (!u.fragment.empty()) {
		fragment = u.fragment;
	}
	if (!u.parameters.empty()) {
		parameters = u.parameters;
	}
}

string url::encode(const std::string &value) const {
	ostringstream escaped;
	unsigned char c;
	const int len = value.length();
	for (int i = 0; i < len; ++i) {
		c = value[i];
		if ((c < 48) ||
		  ((c > 58) && (c < 65)) ||
		  ((c > 91) && (c < 97)) ||
		  (c > 123))
			escaped << "%" << hex << setfill('0') << setw(2) << 
			  uppercase << (unsigned short)c;
		else
			escaped << c;
	}
	return escaped.str();
}

string url::decode(const std::string &value) const {
	string rslt = "";
	const int len = value.length();
	for (int i = 0; i < len; ++i) {      
		if ((value[i] == '%') && ((i + 2) < len) &&
		  isxdigit(value[i + 1]) && isxdigit(value[i + 2])) {
			rslt += char('\0' + strtol(value.substr(i + 1, 2).c_str(), 0, 16));
			i += 2;
		} else {
			if (value[i] == '+')
				rslt += ' ';
			else
				rslt += value[i];
		}
	}
	return rslt;
}

void url::normalize(const std::string &base_path) {
	if ((path.length() > 0) && (path[0] == '/' || path[0] == '\\'))
		path.erase(0, 1);
	string t = base_path + path;
#ifdef WIN32
	// Replace all '\' path windowsisms to RFC compatible '/' ones.
	string::size_type pos = t.find('\\');
	while (pos != string::npos) {
		t.replace(pos, 1, "/");
		pos = t.find('\\');
	}
#endif
	vector<string> v = tokenize()(t, "/");
	vector<string>::reverse_iterator i = v.rbegin();
	path = "";
	int s = 0;
	while (i != v.rend()) {
		if (*i == "..") {
			// skip next element
			s++;
		} else
		if (*i == ".") {
			// do nothing - skip current element
		} else
		if (*i == "") {
			// it's a root path - skip all elements
			break;
		} else {
			if (s > 0) {
				s--;
			} else {
				path = "/" + *i + path;
			}
		}
		++i;
	}
}

std::string url::str() const {
	string rslt = "";
	if (!scheme.empty()) {
		rslt += scheme + "://";
	}
	if (!user.empty()) {
		rslt += user;
		if (!password.empty())
			rslt += ":" + password + "@";
	}
	if (!host.empty()) {
		rslt += host;
		if (port > 0) {
			struct servent *s;
			if ((s = getservbyname(scheme.c_str(), "tcp")) != NULL) {
				int s_port = ntohs(s->s_port);
				if (s_port != port)
					rslt += ":" + to_string<int>(port);
			}
		}
	}
	rslt += get_full_path();
	if (rslt.empty()) {
		rslt += "/";
	}
	return rslt;
}

} // namespace
