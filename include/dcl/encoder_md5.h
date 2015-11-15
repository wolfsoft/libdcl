/*
 * encoder_md5.h
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

#ifndef _ENCODER_MD5_H_
#define _ENCODER_MD5_H_

#include <dcl/codec.h>

namespace dbp {
namespace codec {

//!	MD5 encoder
/*!
	This class implements the
	<a href="http://en.wikipedia.org/wiki/md5">MD5</a> hashing algorithm.
*/
class encoder_md5 {
public:
	//! Calculate MD5 hash
	/*!
		Calculates md5 hash of the data from the input stream and places
		the hash value into the output stream.
		
		\param in the input stream
		\param out the output stream
	*/
	virtual void encode(std::istream &in, std::ostream &out);
};

}} // namespace

#endif /*_ENCODER_MD5_H_*/

