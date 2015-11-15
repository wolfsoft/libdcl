/*
 * atomic_counter.h
 * This file is part of dbPager Server
 *
 * Copyright (C) 2008 - Dennis Prochko <wolfsoft@mail.ru>
 *
 * dbPager Server is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * dbPager Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dbPager Server; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef _ATOMIC_COUNTER_H_
#define _ATOMIC_COUNTER_H_

namespace dbp {

//! Atomic counter class
/*!
	This class represents the one of the synchronization object - the atomic
	counter. The increments and decrements of the counter are serializable,
	so you can use the shared counter in the multithreaded environment.
*/
class atomic_counter {
public:
	//! Constructor
	/*!
		Initialize the counter with zero.
	*/
	atomic_counter(): _counter(0) { }
	//! Constructor
	/*!
		Initialize the counter with value passed.

		\param value the counter initial value.
	*/
	atomic_counter(int value): _counter(value) { }
	//! Copy constructor
	/*!
		Initialize the counter with value of the other counter.

		\param src the source counter.
	*/
	atomic_counter(const atomic_counter &src) {
		_counter = src._counter;
	}
	//! Assignment operator
	const atomic_counter& operator=(const atomic_counter &src) {
		_counter = src._counter;
		return *this;
	}
	//! Increment the counter
	inline void operator++() {
#ifdef __GNUC__
		__sync_add_and_fetch(&_counter, 1);
#endif
	}
	//! Decrement the counter
	inline void operator--() {
#ifdef __GNUC__
		__sync_sub_and_fetch(&_counter, 1);
#endif
	}
private:
	int _counter;
};

} // namespace

#endif /*_ATOMIC_COUNTER_H_*/

