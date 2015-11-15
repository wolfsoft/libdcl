/*
 * url.h
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

#ifndef URL_H_
#define URL_H_

#include <string>

#include <dcl/exception.h>

namespace dbp {

//!	URL parser
/*!
	This class parses Uniform Resource Locator into its parts:

	scheme://[user[:password]@]host[:port]/full_path

	where full_path is: path[?query][#fragment][;parameters]
*/
class url {
public:
	//!	URL scheme - http, https, ftp and so on
	std::string scheme;
	//! Username
	std::string user;
	//! Password
	std::string password;
	//! Host
	std::string host;
	//! Port number
	int port;
	//! URL path
	std::string path;
	//! URL query
	std::string query;
	//! URL fragment
	std::string fragment;
	//! URL parameters
	std::string parameters;
	//! Constructor
	url();
	//! Constructor
	/*!
		Splits a given URL into its parts.
		\param value a URL to parse
	*/ 
	url(const std::string &value);
	//! Constructor
	/*!
		Splits a given URL into its parts.
		\param value a URL to parse
	*/ 
	url(const char *value);
	//! Parse the URL
	/*!
		Splits a given URL into its parts.
		\param value a URL to parse
	*/ 
	void parse(const std::string &value);
	//! URL copy
	/*!
		Copying one URL to another, inheriting missing URL parts.
	*/ 
	void copy_from(const url &u);
	//! Full path getter
	std::string get_full_path() const;
	//! Normalize the URL
	/*
		Expands URL from relative path to absolute path, eleminating all
		'.', '..', '//' sequences.
	*/
	void normalize(const std::string &base_path);
	//! Encode a URL with escape characters
	std::string encode(const std::string &value) const;
	//! Decode a URL from escape characters
	std::string decode(const std::string &value) const;
	//! Reconstruct of a string representation of URL
	std::string str() const;
};

}

#endif /*URL_H_*/
