/*
 * strutils.h
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

#ifndef STRUTILS_H_
#define STRUTILS_H_

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <locale>
#include <cwchar>

#include <dcl/exception.h>
#include <dcl/i18n.h>

namespace dbp {

typedef std::vector<std::string> strings;

//!	Empty string
const std::string nullstr;

//!	Trim trailing spaces from a string
/*!
	The trim class is a functor is used for trimming of trailing
	non-characters symbols (spaces, tabs and CRLFs) from a given string.

	Example:
	\code
string s = dbp:trim()(" Test string\n");
// s is now initialized with "Test string"
	\endcode
*/
class trim: public std::binary_function<std::string, char*, std::string> {
public:
	//!	Trim a given string
	/*!
		\param source a string to trim
		\param delims a delimeter chars (optional)
		\return a trimmed string
	*/
	std::string operator()(const std::string &source,
	  const char *delims = " \t\r\n") const;
};

//!	Case insensitive char comparing
/*!
	The compare functor class is used for case insensitive comparing of two
	chars, with optionally provided locale, for example:
	\code
if (char_compare()(c1, c2)) {...};
	\endcode
*/
class char_compare: public std::binary_function<char, char, bool> {
public:
	//! Constructor
	char_compare(const std::ctype<char> &c): _ct(c) {};
	//! Compare two characters
	/*!
		\param x first char to compare
		\param y second char to compare
		\return true, if x == y, or false otherwise
	*/
	bool operator()(char x, char y) const {
		return _ct.toupper(x) == _ct.toupper(y);
	};
private:
	const std::ctype<char> &_ct;
};

//!	Case insensitive string comparing
/*!
	The compare functor class is used for case insensitive comparing of two
	strings, with optionally provided locale, for example:
	\code
if (compare()(s1, s2)) {...};
	\endcode
*/
class compare: public std::binary_function<std::string, std::string, bool> {
public:
	//!	Constructor
	/*!
		\param L a locale of strings to compare (default: "C" locale)
	*/
	compare(const std::locale &L = std::locale::classic()): loc(L) { };

	compare& operator=(const compare &src) {
		loc = src.loc;
		return *this;
	};

	//!	Compare two strings
	/*!
		\param x first string to compare
		\param y second string to compare
		\return true, if x == y, or false otherwise
	*/
	bool operator()(const std::string &x, const std::string &y) const;
private:
	std::locale loc;
};

//!	Split the string to tokens
/*!
	The tokenize functor class splits a string by a delimiting characters to
	a separate tokens.
*/
class tokenize {
public:
	//!	Split a string to a vector of tokens
	/*!
		\param source a source string
		\param delims an optional delimiting characters
		\return a vector of tokens
	*/
	strings operator()(const std::string &source,
	  const char *delims = ",;");
	//!	Split a string to a token pair
	/*!
		Delimiting characters are searching from the end of the string to the
		beginning when greedy = true, or from the beginning of the string
		otherwise. If one is found, string is splitting into pair of a tokens.

		\param source a source string
		\param left (out) the first element of tokens pair
		\param right (out) the second element of tokens pair
		\param delims an optional delimiting characters
	*/
	void operator()(const std::string &source, std::string &left,
	  std::string &right, bool greedy = true, const char *delims = "/\\");
};

//!	String format
/*!
	The format class formats a string by replacing of parameter markers
	by parameter values. Each parameter marker should be in the
	following format: {x}, where "x" - is a parameter number as passed to
	the class. Parameters are assigned by % operator, for example:
	\code
int i = 1;
// prints "The 1-st test string"
cout << dbp::format("The {0}-st {1} string") % i % "test";
	\endcode
*/
class format {
private:
	std::string str_;
	strings params_;
public:
	//!	Constructor
	/*!
		\param source a string to format
	*/
	format(const std::string &source): str_(source) {};
	//! Format a string
	/*!
		\return a formatted string
	*/
	std::string str() const;
	//! Assign the formatting parameter
	/*!
		\param x a formatting parameter
	*/
	template<class T>
	format& operator%(const T& x) {
		std::stringstream s;
		s << x;
		params_.push_back(s.str());
		return *this;
	}
	//! Format a string
	/*!
		Formats a string and outputs to ostream-compatible class.
	*/
	friend std::ostream& operator<<(std::ostream&, format&);
};

/*
Regular expression string iterator
class regexp {
public:
	class iterator: public std::iterator<std::input_iterator_tag,
	  int> {
	public:
		int pos;
		std::string found;
		explicit iterator(regexp &expression): expr(expression) { };
	private:
		regexp &expr;
	};
	iterator operator()(const std::string &source,
	  const std::string &expression) {
		_source = source;
		_expression = expression;
		return iterator(*this);
	};
private:
	strings _regexp;
	std::string _source, _expression;
};
*/

//!	String to wide string convert
/*!
	The string2wstring functor class is used for converting strings to widen
	(unicode) strings according specified locale.
*/
class string2wstring: public std::binary_function<std::string, std::locale const &,
  std::wstring> {
public:
	//! Convert string to the wide string
	/*!
		\param source the string to convert
		\param loc the string locale (default: current locale)
		\return the converted wide string
	*/
	std::wstring operator()(const std::string &source,
	  std::locale const &loc = std::locale()) const;
};

//!	Wide string to string convert
/*!
	The wstring2string functor class is used for converting widen (unicode)
	strings to strings according specified locale.
*/
class wstring2string: public std::binary_function<std::wstring, std::locale const &,
  std::string> {
public:
	//! Convert wide string to the string
	/*!
		\param source the wide string to convert
		\param loc the string locale (default: current locale)
		\return the converted string
	*/
	std::string operator()(const std::wstring &text,
	  std::locale const &loc = std::locale()) const;
};

//!	Get lexicographical string length
/*!
	The chars_count functor class is used to calculate the string length in
	characters (not bytes) according specified locale.
*/
class char_length: public std::binary_function<std::string, std::locale&,
  std::size_t> {
public:
	//! Get length
	/*!
		\param text the source string
		\param loc the string locale (default: current locale)
		\return the count of characters in the source string
	*/
	std::size_t operator()(std::string const &text,
	  std::locale const &loc = std::locale("")) const;
};

//!	Type conversion exception class
/*!
	The type_conversion_exception class is used to signal about error
	when type conversion.
*/
class type_conversion_exception: public dbp::exception { };

//!	Convert of a given type to a string type
/*!
	The to_string template function converts the value of any type to string.
*/
template <class TYPE>
std::string to_string(const TYPE value) throw(type_conversion_exception) {
	std::ostringstream s;
	s.imbue(std::locale::classic());
	try {
		s << value;
	}
	catch (...) {
		throw type_conversion_exception();
	}
	return s.str();
}

template <>
std::string to_string<const char*>(const char *value) throw(type_conversion_exception);

template <>
std::string to_string<const std::string&>(const std::string &value) throw(type_conversion_exception);

template <>
std::string to_string<int>(int value) throw(type_conversion_exception);

template <>
std::string to_string<void*>(void *value) throw(type_conversion_exception);

/*
template <>
std::string to_string<float>(float value) throw(type_conversion_exception);

template <>
std::string to_string<double>(double value) throw(type_conversion_exception);
*/
//!	Convert of a string to a given type
/*!
	The from_string template function converts the string value to value of
	any type.
*/
template <class TYPE>
TYPE from_string(const std::string &value) throw(type_conversion_exception) {
	std::istringstream s(value);
	s.imbue(std::locale::classic());
	TYPE rslt;
	try {
  		if (!(s >> rslt))
			throw type_conversion_exception();
	}
	catch (...) {
		throw type_conversion_exception();
	}
	return rslt;
}

template <>
const char* from_string<const char*>(const std::string &value)
  throw(type_conversion_exception);

template <>
std::string from_string<std::string>(const std::string &value)
  throw(type_conversion_exception);

template <>
int from_string<int>(const std::string &value)
  throw(type_conversion_exception);

template <>
void* from_string<void*>(const std::string &value)
  throw(type_conversion_exception);

template <>
float from_string<float>(const std::string &value)
  throw(type_conversion_exception);

template <>
double from_string<double>(const std::string &value)
  throw(type_conversion_exception);

} // namespace

#endif /*STRUTILS_H_*/
