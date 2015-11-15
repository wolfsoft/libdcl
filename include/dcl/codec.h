/*
 * codec.h
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

#ifndef _CODEC_H_
#define _CODEC_H_

#include <iostream>

#include <dcl/exception.h>

namespace dbp {
namespace codec {

//! Coder/Decoder exception
/*!
	This class represents an exception in the encoder, decoder, codec classes.
*/
class codec_exception: public dbp::exception {
public:
	//! Constructor
	codec_exception(const std::string &msg): exception(msg) { };
};

//! Data encoder interface
/*!
	This class is an interface to the data encoders.
*/
class encoder {
public:
	//! Destructor
	virtual ~encoder() { }
	//! Encode data
	/*!
		Encodes data from the input stream and puts the encoded data to the
		output stream.
	*/
	virtual void encode(std::istream &in, std::ostream &out) = 0;
};

//! Data decoder interface
/*!
	This class is an interface to the data decoders.
*/
class decoder {
public:
	//! Destructor
	virtual ~decoder() { }
	//! Decode data
	/*!
		Decodes data from the input stream and puts the decoded data to the
		output stream.
	*/
	virtual void decode(std::istream &in, std::ostream &out) = 0;
};

//!	Codec (coder/decoder) interface
/*!
	This class is an interface to the data codecs.
*/
class codec: public encoder, public decoder { };

}} // namespace

#endif /*_CODEC_H_*/

