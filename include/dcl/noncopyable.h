/*
 * noncopyable.h
 * This file is part of dbPager Classes Library (DCL)
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
 
#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

namespace dbp {

//! Non-copyable class
/*!
	To make your class noncopyable, simply subclass from this.
*/
class noncopyable {
protected:
	noncopyable() { };
	~noncopyable() { };
private:
	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
};

} //namespace

#endif /*_NONCOPYABLE_H_*/
