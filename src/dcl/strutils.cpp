/*
 * strutils.cpp
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

#include <float.h>
#include <ctype.h>
#include <math.h>
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>

#include <dcl/strutils.h>

namespace dbp {

using namespace std;

string trim::operator()(const string &source, const char *delims) const {
	string result(source);
	string::size_type index = result.find_last_not_of(delims);
	if (index != string::npos)
		result.erase(++index);
	index = result.find_first_not_of(delims);
	if (index != string::npos)
		result.erase(0, index);
	else
		result.erase();
	return result;
}

bool compare::operator()(const std::string &x, const std::string &y) const {
	const std::ctype<char> &ct = std::use_facet<std::ctype<char> >(loc);
	if (x.length() != y.length())
		return false;
	else
		return equal(x.begin(), x.end(), y.begin(), char_compare(ct));
}

vector<string> tokenize::operator()(const string &source, const char *delims) {
	vector<string> rslt;
	if (!source.empty()) {
		string::size_type pos = 0, old_pos = 0;
		while ((pos = source.find_first_of(delims, old_pos)) != string::npos) {
			rslt.push_back(trim()(source.substr(old_pos, pos - old_pos)));
			old_pos = pos + 1;
		}
		rslt.push_back(trim()(source.substr(old_pos, source.length())));
	}
	return rslt;
}

void tokenize::operator()(const std::string &source, std::string &left,
  std::string &right, const char *delims) {
	if (!source.empty()) {
		string::size_type pos = source.find_last_of(delims);
		if (pos == string::npos) {
			left = "";
			right = source;
		} else {
			left = source.substr(0, pos);
			right = source.substr(pos + 1, source.length() - pos - 1);
		}
	}
}

string format::str() const {
	string rslt;
	string::size_type pos = 0, old_pos = 0;
	// searching for brackets
	while ((pos = str_.find_first_of("{}", old_pos)) != string::npos) {
		// if opening bracket found, copy string before bracket;
		// if closing bracket found, copy parameter from a list by number
		if (str_[pos] == '{')
			rslt += str_.substr(old_pos, pos - old_pos);
		else
			rslt += params_[from_string<int>(str_.substr(old_pos,
			  pos - old_pos))];
		old_pos = pos + 1;
	}
	// copy remaining string
	rslt += str_.substr(old_pos, str_.length());
	return rslt;
}

ostream& operator<<(ostream &out, format &f) {
	return (out << f.str());
}

std::wstring string2wstring::operator()(const std::string &source,
  std::locale const &loc) const {
	wstring result(source.size(), wchar_t());
	const ctype<wchar_t>& ct = use_facet<ctype<wchar_t> >(loc);
	wstring::iterator dest(result.begin());
	string::const_iterator it(source.begin());
	string::const_iterator end(source.end());
	for (; it != end; ++it, ++dest)
		*dest = ct.widen(*it);
	return result;
}

std::string wstring2string::operator()(const std::wstring &source,
  std::locale const &loc) const {
	std::string result(source.size(), char());
	const ctype<char>& ct = use_facet<ctype<char> >(loc);
	string::iterator dest(result.begin());
	wstring::const_iterator it(source.begin());
	wstring::const_iterator end(source.end());
	for (; it != end; ++it, ++dest)
		*dest = ct.narrow(*it, '?');
	return result;
}

std::size_t char_length::operator()(std::string const &text,
  std::locale const &loc) const {
	std::size_t len = text.length();
	char const* input = text.data();
	// get the conversion facet of the locale
	typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_type;
	cvt_type const& cvt = std::use_facet<cvt_type>(loc);
	// allocate buffer for conversion destination
	std::size_t bufsize = cvt.max_length() * len;
	wchar_t* destbuf = new wchar_t[bufsize];
	wchar_t* dest_end;
	// do the conversion
	mbstate_t state = mbstate_t();
	cvt.in(state, input, input + len, input, destbuf, destbuf + bufsize, dest_end);
	// determine the length of the converted sequence
	std::size_t length = dest_end - destbuf;
	// get rid of the buffer
	delete[] destbuf;
	// return the result
	return length;
}

template <>
std::string to_string(const char *value) throw(type_conversion_exception) {
	return value ? std::string(value) : "";
}

template <>
std::string to_string(const std::string &value) throw(type_conversion_exception) {
	return value;
}

template <>
std::string to_string(int value) throw(type_conversion_exception) {
	char buf[64];
	snprintf(buf, 64, "%d", value);
	return string(buf);
}

template <>
std::string to_string(void* value) throw(type_conversion_exception) {
	char buf[64];
	snprintf(buf, 64, "%p", (intptr_t)value);
	return string(buf);
}

/*
template <>
std::string to_string(float value) throw(type_conversion_exception) {
	char buf[4 + FLT_MANT_DIG - FLT_MIN_EXP];
	snprintf(buf, 4 + FLT_MANT_DIG - FLT_MIN_EXP, "%f", value);
	return string(buf);
}

template <>
std::string to_string(double value) throw(type_conversion_exception) {
	char buf[4 + DBL_MANT_DIG - DBL_MIN_EXP];
	snprintf(buf, 4 + DBL_MANT_DIG - DBL_MIN_EXP, "%lf", value);
	return string(buf);
}
*/
template <>
const char* from_string(const std::string &value) throw(type_conversion_exception) {
	return value.c_str();
}

template <>
std::string from_string(const std::string &value) throw(type_conversion_exception) {
	return value;
}

double my_strtod(const char *str, char **endptr) {
	double number;
	int exponent;
	int negative;
	char *p = (char *) str;
	double p10;
	int n;
	int num_digits;
	int num_decimals;

	// Skip leading whitespace
	while (isspace(*p)) p++;

	// Handle optional sign
	negative = 0;
	switch (*p) {
		case '-': negative = 1; // Fall through to increment position
		case '+': p++;
	}

	number = 0.;
	exponent = 0;
	num_digits = 0;
	num_decimals = 0;

	// Process string of digits
	while (isdigit(*p)) {
		number = number * 10. + (*p - '0');
		p++;
		num_digits++;
	}

	// Process decimal part
	if (*p == '.') {
		p++;

		while (isdigit(*p)) {
			number = number * 10. + (*p - '0');
			p++;
			num_digits++;
			num_decimals++;
		}

		exponent -= num_decimals;
	}

	if (num_digits == 0) {
		throw type_conversion_exception();
	}

	// Correct for sign
	if (negative) number = -number;

	// Process an exponent string
	if (*p == 'e' || *p == 'E') {
		// Handle optional sign
		negative = 0;
		switch (*++p) {
			case '-': negative = 1;	 // Fall through to increment pos
			case '+': p++;
		}

		// Process string of digits
		n = 0;
		while (isdigit(*p)) {
			n = n * 10 + (*p - '0');
			p++;
		}

		if (negative) {
			exponent -= n;
		} else {
			exponent += n;
		}
	}

	if (exponent < DBL_MIN_EXP	|| exponent > DBL_MAX_EXP) {
		throw type_conversion_exception();
	}

	// Scale the result
	p10 = 10.;
	n = exponent;
	if (n < 0) n = -n;
	while (n) {
		if (n & 1) {
			if (exponent < 0) {
				number /= p10;
			} else {
				number *= p10;
			}
		}
		n >>= 1;
		p10 *= p10;
	}

	if (number == HUGE_VAL)
		throw type_conversion_exception();
	if (endptr) *endptr = p;

	return number;
}

template <>
int from_string(const std::string &value) throw(type_conversion_exception) {
	return strtol(value.c_str(), NULL, 0);
}

template <>
void* from_string(const std::string &value) throw(type_conversion_exception) {
	return (void*)strtoull(value.c_str(), NULL, 0);
}

template <>
float from_string(const std::string &value) throw(type_conversion_exception) {
	return (float)my_strtod(value.c_str(), NULL);
}

template <>
double from_string(const std::string &value) throw(type_conversion_exception) {
	return my_strtod(value.c_str(), NULL);
}

} // namespace
