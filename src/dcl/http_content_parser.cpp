/*
 * http_content_parser.cpp
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

#include <memory>

#include "dcl/factory.h"
#include "dcl/http_content_parser.h"
#include "dcl/strutils.h"
#include "dcl/url.h"

namespace dbp {

using namespace std;

// abstract parser interface
class http_parser {
public:
	virtual http_content_elements parse() = 0;
	void set_content(const char *buffer, size_t size) {
		buf = buffer;
		buf_size = size;
	}
protected:
	const char *buf;
	size_t buf_size;
};

// generic content parser interface (no parsing)
class http_generic_parser: public http_parser {
public:
	virtual http_content_elements parse() {
		http_content_elements elements;
		if (buf && buf_size > 0) {
			http_content_element element;
			element.name = "content";
			element.value.reserve(buf_size);
			element.value.insert(0, buf, buf_size);
			elements.push_back(element);
		}
		return elements;
	}
};

// application/x-www-form-urlencoded parser
class http_urlencoded_parser: public http_parser {
public:
	virtual http_content_elements parse() {
		http_content_elements elements;
		if (buf) {
			http_content_element element;
			// split content to tokens
			strings pairs = tokenize()(buf, "&");
			for (strings::const_iterator i = pairs.begin();
			  i != pairs.end(); ++i) {
				// split each token to name and value
				string param, value;
				tokenize()(*i, param, value, false, "=");
				// decode urlencoded value
				element.name = url().decode(param);
				element.value = url().decode(value);
				// add to result
				elements.push_back(element);
			}
		}
		return elements;
	}
};

// multipart/form-data parser
class http_form_data_parser: public http_parser {
public:
	virtual http_content_elements parse() {
		http_content_elements elements;
		if (buf) {
		}
		return elements;
	}
};

// concrete parser builder
class http_content_parser::impl {
public:
	// registering of concrete parsers
	impl(const http_header& h): header(h) {
		f.register_class<http_form_data_parser>("multipart/form-data");
		f.register_class<http_urlencoded_parser>("application/x-www-form-urlencoded");
	}
	// create concrete parser instance by http content type
	http_parser* create_parser() {
		// parse content type
		string ct, cv;
		tokenize()(header.get_content_type(), ct, cv, false, ";");
		ct = trim()(ct);
		cv = trim()(cv);
		// create concrete parser
		http_parser *p = NULL;
		try {
			p = f.create(ct);
		}
		catch (...) {
			if (!p)
				p = new http_generic_parser();
			else
				throw;
		}
		// initialize the parser
		p->set_content(header.get_content(), header.get_content_size());
		return p;
	}
private:
	const http_header& header;
	factory<http_parser> f;
};

// http_content_parser

http_content_parser::http_content_parser(const http_header& header) {
	pimpl = new impl(header);
}

http_content_parser::~http_content_parser() {
	delete pimpl;
}

http_content_elements http_content_parser::parse() const {
	auto_ptr<http_parser> parser(pimpl->create_parser());
	return parser->parse();
}

} // namespace

