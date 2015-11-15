/*
 * codec_base64.h
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

#ifndef _CODEC_BASE64_H_
#define _CODEC_BASE64_H_

#include <dcl/codec.h>

namespace dbp {
namespace codec {

//!	Base64 coder/decoder class.
/*!
	This class implements the
	<a href="http://en.wikipedia.org/wiki/Base64">base64</a> encoding
	algorithm.
*/
class codec_base64 {
public:
	//! Base64 encode
	/*!
		Encodes the data from the input stream and places encoded data
		into the output stream.
		
		\param in the input stream
		\param out the output stream
	*/
	virtual void encode(std::istream &in, std::ostream &out);
	//! Base64 decode
	/*!
		Decodes the base64 encoded data from the input stream and places
		decoded data into the output stream.
		
		\param in the input stream
		\param out the output stream
	*/
	virtual void decode(std::istream &in, std::ostream &out);
};

}} // namespace

#endif /*_CODEC_BASE64_H_*/

