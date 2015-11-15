/*
 * filefs.h
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

#ifndef _FILEFS_H_
#define _FILEFS_H_

#include <string>
#include <iterator>

#include <dcl/filesystem.h>

namespace dbp {

//!	Local filesystem class
/*!
	This class provides methods to access the local file system.
*/
class filefs: public filesystem {
public:
	//! Constructor
	filefs();
	//! Destructor
	virtual ~filefs();
	virtual std::string get_path_delimiter() const;
	virtual std::string get_current_dir() const;
	virtual void set_current_dir(const std::string &name) const;
	virtual std::string get_temp_dir() const;
	virtual std::string get_home_dir() const;
	virtual std::string get_locale_dir() const;
	virtual std::string get_system_config_dir() const;
	virtual std::string get_data_dir() const;
private:
	filesystem *pimpl;
	virtual bool find_first_file(const std::string &pattern,
	  std::string &filename);
	virtual bool find_next_file(std::string &filename);
};

}

#endif /*_FILEFS_H_*/

