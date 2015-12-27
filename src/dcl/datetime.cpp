/*
 * datetime.cpp
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

#include <locale.h>
#include <dcl/datetime.h>

#ifdef _WIN32
#include "win32/strptime.h"
#endif

namespace dbp {

using namespace std;

datetime::datetime(const std::string &src, const std::string &fmt): _t(0) {
	struct tm tm;
	strptime(src.c_str(), fmt.c_str(), &tm);
	tm.tm_isdst = 1;
	_t = mktime(&tm);
}

int datetime::operator-(const datetime& rhs) {
	return difftime(_t, rhs._t);
}

datetime& datetime::now() {
	_t = time(NULL);
	return *this;
}

int datetime::year() const {
	struct tm *tm = localtime(&_t);
	return tm->tm_year + 1900;
}

datetime& datetime::year(int val) {
	struct tm *tm = localtime(&_t);
	tm->tm_year = val - 1900;
	_t = mktime(tm);
	return *this;
}

int datetime::month() const {
	struct tm *tm = localtime(&_t);
	return tm->tm_mon + 1;
}

datetime& datetime::month(int val) {
	struct tm *tm = localtime(&_t);
	tm->tm_mon = val - 1;
	_t = mktime(tm);
	return *this;
}

int datetime::day() const {
	struct tm *tm = localtime(&_t);
	return tm->tm_mday;
}

datetime& datetime::day(int val) {
	struct tm *tm = localtime(&_t);
	tm->tm_mday = val;
	_t = mktime(tm);
	return *this;
}

int datetime::hour() const {
	struct tm *tm = localtime(&_t);
	return tm->tm_hour;
}

datetime& datetime::hour(int val) {
	struct tm *tm = localtime(&_t);
	tm->tm_hour = val;
	_t = mktime(tm);
	return *this;
}

int datetime::minute() const {
	struct tm *tm = localtime(&_t);
	return tm->tm_min;
}

datetime& datetime::minute(int val) {
	struct tm *tm = localtime(&_t);
	tm->tm_min = val;
	_t = mktime(tm);
	return *this;
}

int datetime::second() const {
	struct tm *tm = localtime(&_t);
	return tm->tm_sec;
}

datetime& datetime::second(int val) {
	struct tm *tm = localtime(&_t);
	tm->tm_sec = val;
	_t = mktime(tm);
	return *this;
}

void datetime::clear() {
	_t = 0;
}

bool datetime::empty() const {
	return (_t == 0);
}

std::string datetime::str(const std::string &format) const {
	struct tm *tm = localtime(&_t);
	char buffer[128];
	strftime(buffer, 128, format.c_str(), tm);
	return std::string(buffer);
}

std::string datetime::as_gmt() const {
	struct tm *tm = gmtime(&_t);
	char buffer[128];
	locale_t loc, old_loc;
	
	loc = newlocale(LC_ALL_MASK, "C", NULL);
	old_loc = uselocale(loc);
	freelocale(loc);
	strftime(buffer, 128, "%a, %d %b %Y %H:%M:%S GMT", tm);
	uselocale(old_loc);

	return std::string(buffer);
}
	
} // namespace

