/*
 * socket_impl.cpp
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

#include <windows.h>
#include <winsock2.h>

#include <dcl/singleton.h>

namespace dbp {
namespace local {

using namespace std;

class winsock2_init: public singleton<winsock2_init> {
	friend class singleton<winsock2_init>;
public:
	virtual ~winsock2_init() {
		WSACleanup();
	};
private:
	winsock2_init() {
		WSAStartup(MAKEWORD(2, 2), &_WSAData);
	};
	winsock2_init(const winsock2_init&) {
		WSAStartup(MAKEWORD(2, 2), &_WSAData);
	};
	WSADATA _WSAData;
};

}} // namespace

