/*
 * mimetype.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2008-2009 Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _MIMETYPE_H_
#define _MIMETYPE_H_

#include <map>
#include <string>

namespace dbp {

//!	MIME type file classifier.
/*!
	Detects the MIME type of the given file.
*/
class mimetype {
public:
	//! Constructor
	mimetype();
	//! Register custom MIME type
	/*!
		Registers a MIME type for a given file extension.
		
		\param ext the file extension (without dot)
		\param mime_type the MIME file type
	*/
	void register_type(const std::string &ext, const std::string &mime_type);
	//! Operator ()
	/*!
		Detects the MIME type of the given file.
		
		\filename is a path (or name) of the file
		\returns the MIME type of the file
	*/
	const std::string& operator()(const std::string& filename) const;
private:
	typedef std::map<std::string, std::string> mime_types;
	mime_types mime;
	std::string default_type;
	void load_from_file(const std::string &filename);
};

}

#endif /*_MIMETYPE_H_*/
