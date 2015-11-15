/*
 * uuid.cpp
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
#include <cstdlib>
#include <cstring>

#include <dcl/uuid.h>
#include <dcl/singleton.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

namespace internal {

class rand_seed: public singleton<rand_seed> {
	friend class singleton<rand_seed>;
private:
	rand_seed() {
		srand(time(NULL));
	}
	rand_seed(const rand_seed&) {
		rand_seed();
	}
};
 
} // namespace

// friend IO operators

istream& operator>>(istream &in, uuid &value) {
	string s;
	in >> s;
	value.assign(s);
	return in;
}

ostream& operator<<(ostream &out, const uuid &value) {
	out << value.str();
	return out;
}

// uuid

uuid::uuid() {
	generate();
}

uuid::uuid(const std::string &value) {
	assign(value);
}

uuid::uuid(const char *value) {
	if (value)
		assign(value);
	else
		clear();
}

uuid::uuid(const uuid &rhs) {
	std::copy(rhs._data, rhs._data + size, _data);
}

uuid& uuid::operator=(const uuid &rhs) {
	std::copy(rhs._data, rhs._data + size, _data);
	return *this;
}

bool uuid::operator==(const uuid &rhs) const {
	return std::equal(_data, _data + size, rhs._data);
}

bool uuid::operator<(const uuid &rhs) const {
	return std::lexicographical_compare(_data, _data + size,
	  rhs._data, rhs._data + size);
}

void uuid::clear() {
	std::fill(_data, _data + size, 0);
}

bool uuid::empty() const {
	for (int i = 0; i < size; ++i)
		if (_data[i])
			return false;
	return true;
}

void uuid::assign(const std::string &value) {
	clear();
	string buf;
	int len = value.length();
	int j = 0;
	for (int i = 0; i < len; ++i) {
		if (isxdigit(value[i]))
			buf += value[i];
		else
			continue;
		if (buf.length() == 2) {
			_data[j] = strtol(buf.c_str(), NULL, 16);
			buf.clear();
			j++;
			if (j == size)
				return;
		}
	}
	if (!buf.empty())
		_data[j] = strtol(buf.c_str(), NULL, 16);
}

void uuid::generate() {
	// initialize random generator seed if needed
	internal::rand_seed::instance();
	// fill first bytes with current time
	time_t t = time(NULL);
	// but copy only meaningful 32 bytes or less if it is
	int len = sizeof(t) > 4 ? 4 : sizeof(t);
	memcpy(&_data, &t, len);
	// fill remaining bytes with random numbers
	for (int i = len; i < size; ++i) {
		_data[i] = rand() % 0xFF;
	}
}

std::string uuid::str() const {
	stringstream s;
	for (int i = 0; i < size; ++i) {
		s << setw(2) << setfill('0') << hex << unsigned(_data[i]);
		if ((i == 3) || (i == 5) || (i == 7) || (i == 9))
			s << '-';
	}
	return s.str();
}

} // namespace

