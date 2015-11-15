/*
 * codec_base64.cpp
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

 * This file contains a code available on the following license:
---
LICENCE:	Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

			Permission is hereby granted, free of charge, to any person
			obtaining a copy of this software and associated
			documentation files (the "Software"), to deal in the
			Software without restriction, including without limitation
			the rights to use, copy, modify, merge, publish, distribute,
			sublicense, and/or sell copies of the Software, and to
			permit persons to whom the Software is furnished to do so,
			subject to the following conditions:

			The above copyright notice and this permission notice shall
			be included in all copies or substantial portions of the
			Software.
---
 */

#include <dcl/codec_base64.h>

namespace dbp {
namespace codec {

using namespace std;

const int linesize = 72;

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
	out[0] = cb64[ in[0] >> 2 ];
	out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
	out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
	out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
void decodeblock( unsigned char in[4], unsigned char out[3] )
{   
	out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
	out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
	out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

void codec_base64::encode(istream &in, ostream &out) {
	unsigned char _in[3], _out[4];
	int i, len, blocksout = 0;
	while (!in.eof()) {
		len = 0;
		for (i = 0; i < 3; i++) {
			in.get((char&)_in[i]);
			if(!in.eof()) {
				len++;
			}
			else {
				_in[i] = 0;
			}
		}
		if (len) {
			encodeblock(_in, _out, len);
			for (i = 0; i < 4; i++) {
				out.put(_out[i]);
			}
			blocksout++;
		}
		if (blocksout >= (linesize / 4) || in.eof()) {
			if (blocksout) {
				out << endl;
			}
			blocksout = 0;
		}
	}
}

void codec_base64::decode(istream &in, ostream &out) {
	unsigned char _in[4], _out[3], v;
	int i, len;
	while (!in.eof()) {
		for (len = 0, i = 0; i < 4 && !in.eof(); i++) {
			char v = 0;
			while (!in.eof() && v == 0) {
				in.get((char&)v);
				v = (unsigned char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);
				if (v) {
					v = (unsigned char)((v == '$') ? 0 : v - 61);
				}
			}
			if (!in.eof()) {
				len++;
				if (v) {
					_in[i] = (unsigned char)(v - 1);
				}
			}
			else {
				_in[i] = 0;
			}
		}
		if (len > 0) {
			decodeblock(_in, _out);
			for (i = 0; i < len - 1; i++) {
				out.put(_out[i]);
			}
		}
	}
}

}} // namespace

