/*
 * datetime.h
 * This file is part of DCL
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

#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <ctime>
#include <string>

namespace dbp {

//! Default date and time format
#define DEFAULT_DATETIME_FORMAT "%a, %d %b %Y %H:%M:%S %Z"

//!	Date and time class
/*!
	This class represents the date and time.
*/
class datetime {
public:
	//! Constructor
	datetime(): _t(0) { };
	//! Constructor
	datetime(const std::string &src, const std::string &fmt =
	  DEFAULT_DATETIME_FORMAT);
	//! Copy constructor
	datetime(const datetime &rhs) {
		_t = rhs._t;
	}
	//! Assignment operator
	datetime& operator=(const datetime &rhs) {
		_t = rhs._t;
		return *this;
	}
	//! Comparison operator
	bool operator==(const datetime &rhs) const {
		return (_t == rhs._t);
	}
	//! Comparison operator
	bool operator<(const datetime &rhs) const {
		return _t < rhs._t;
	}
	//! Comparison operator
	bool operator>(const datetime &rhs) const {
		return (rhs < *this);
	}
	//! Comparison operator
	bool operator<=(const datetime &rhs) const {
		return !(rhs < *this);
	}
	//! Comparison operator
	bool operator>=(const datetime& rhs) const {
		return !(*this < rhs);
	}
	//! Comparison operator
	bool operator!=(const datetime &rhs) const {
		return !(*this == rhs);
	}
	//! Comparison operator (check for empty value)
	bool operator!() const {
		return empty();
	}
	//! Minus operator (difference in seconds)
	int operator-(const datetime& rhs);
	typedef bool (datetime::*bool_type)() const;
	//! Comparison operator (safe bool)
	operator bool_type() const {
		return empty() ? NULL : &datetime::empty;
	}
	//! Set to current time
	datetime& now();
	//! Get year of the date
	int year() const;
	//! Set year of the date
	datetime& year(int val);
	//! Get month of the date
	int month() const;
	//! Set month of the date
	datetime& month(int val);
	//! Get day of the date
	int day() const;
	//! Set day of the date
	datetime& day(int val);
	//! Get hour of the date
	int hour() const;
	//! Set hour of the date
	datetime& hour(int val);
	//! Get minute of the date
	int minute() const;
	//! Set minute of the date
	datetime& minute(int val);
	//! Get second of the date
	int second() const;
	//! Set second of the date
	datetime& second(int val);
	//! Clear (set to empty value)
	void clear();
	//! Check for empty value
	bool empty() const;
	//! Convert to local time string representation with the current locale
	/*!
		\param format a string containing any combination of regular characters
		and special format specifiers. These format specifiers are replaced by
		the function to the corresponding values to represent the time.
		They all begin with a percentage (%) sign, and are:
		\verbatim
Specifier   Replaced by                         Example
-------------------------------------------------------------------------
%a          Abbreviated weekday name *          Thu
%A          Full weekday name *                 Thursday
%b          Abbreviated month name *            Aug
%B          Full month name *                   August
%c          Date and time representation *      Thu Aug 23 14:55:02 2001
%d          Day of the month (01-31)            23
%H          Hour in 24h format (00-23)          14
%I          Hour in 12h format (01-12)          02
%j          Day of the year (001-366)           235
%m          Month as a decimal number (01-12)   08
%M          Minute (00-59)                      55
%p          AM or PM designation                PM
%S          Second (00-61)                      02
%U          Week number with the first Sunday
            as the first day of
            week one (00-53)                    33
%w          Weekday as a decimal number with
            Sunday as 0 (0-6)                   4
%W          Week number with the first Monday
            as the first day of
            week one (00-53)                    34
%x          Date representation *               08/23/01
%X          Time representation *               14:55:02
%y          Year, last two digits (00-99)       01
%Y          Year                                2001
%Z          Timezone name or abbreviation       CDT
%%          A % sign                            %
-------------------------------------------------------------------------
* The specifiers whose description is marked with an asterisk (*) are
  locale-dependent.
		\endverbatim
	*/
	std::string str(const std::string &format = DEFAULT_DATETIME_FORMAT) const;
	//! Convert to RFC 2822 date/time string representation, i.e.
	//!	"Sat, 16 May 2009 21:25:26 GMT"
	std::string as_gmt() const;
private:
	time_t _t;
};

}

#endif /*_DATETIME_H_*/

