/*
 * http_content_parser.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2010 Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _HTTP_CONTENT_PARSER_H_
#define _HTTP_CONTENT_PARSER_H_

#include <vector>

#include <dcl/http_header.h>

namespace dbp {

class http_content_element {
public:
	std::string name;
	std::string value;
};

typedef std::vector<http_content_element> http_content_elements;

class http_content_parser {
public:
	http_content_parser(const http_header& header);
	virtual ~http_content_parser();
	http_content_elements parse() const;
private:
	class impl;
	impl *pimpl;
};

} // namespace

#endif /*_HTTP_CONTENT_PARSER_H_*/

