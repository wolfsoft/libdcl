/*
 * filesystem.h
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

#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <iterator>
#include <string>

namespace dbp {

//! Filesystem interface
/*!
	This class provides an interface for file systems implementations.
*/
class filesystem {
	friend class const_iterator;
	friend class filefs;
public:
	//! Filesystem search results iterator
	class const_iterator: public std::iterator<
	  std::forward_iterator_tag,
	  std::string> {
		friend class filesystem;
	public:
		const_iterator& operator++() {
			_rownum++;
			if (!_fs) {
				_rownum = 0;
			}
			else
			if (!_fs->find_next_file(_filename)) {
				_rownum = 0;
			}
			return *this;
		};
		const_iterator operator++(int) {
			const_iterator save = *this;
			++*this;
			return save;
		};
		const std::string* operator->() const {
			return &**this;
		};
		const std::string& operator*() const {
			return _filename;
		};
		bool operator==(const const_iterator &value) const {
			return (_rownum == value._rownum);
		};
		bool operator!=(const const_iterator &value) const {
			return (_rownum != value._rownum);
		};
	private:
		int _rownum;
		std::string _filename;
		filesystem *_fs;
		const_iterator(): _rownum(0), _fs(NULL) { };
		const_iterator(filesystem *fs, const std::string &pattern):
		  _rownum(0), _fs(fs) {
			if (_fs->find_first_file(pattern, _filename))
				_rownum++;
		};
	};
	//! Destructor
	virtual ~filesystem() { };
	//! Obtain default path delimiter
	/*!
		Returns the symbol(s) used for delimiting path elements in
		the current operating system.
	*/
	virtual std::string get_path_delimiter() const = 0;
	//! Determine the current directory
	/*!
		Returns the current directory path.
	*/
	virtual std::string get_current_dir() const = 0;
	//! Set the current directory
	/*!
		Changes the current directory
	*/
	virtual void set_current_dir(const std::string &name) const = 0;
	//! Obtain a temporary directory
	/*!
		Returns the system temporary directory path. It should be
		suitable path for creating of temporary files.
	*/
	virtual std::string get_temp_dir() const = 0;
	//! Obtain current user's home directory
	/*!
		Returns the current user home directory path (aka profile).
	*/
	virtual std::string get_home_dir() const = 0;
	//! Obtain the locale-dependent data directory
	/*!
		Returns the directory where localization files are installed.
	*/
	virtual std::string get_locale_dir() const = 0;
	//! Obtain the directory with configuration files
	/*!
		Returns the directory where configuration files are kept.
	*/
	virtual std::string get_system_config_dir() const = 0;
	//! Obtain the application data directory
	/*!
		Returns the directory where application data files (images, icons,
		resources...) are kept.
	*/
	virtual std::string get_data_dir() const = 0;
	//! Search a file by mask
	/*!
		Provides an iterator of file search results, specified by
		mask (e.g. "*.*").

		\param pattern the search pattern (mask)
		\returns the search results iterator, pointed to the first result found
	*/
	const_iterator find(const std::string &pattern) {
		return const_iterator(&*this, pattern);
	}
	//! Points to the end of the file search list
	/*!
		Provides an iterator of file search results (the last result found)

		\returns the search results iterator, pointed to the last result found
	*/
	const_iterator end() {
		return const_iterator();
	}
protected:
	virtual bool find_first_file(const std::string &pattern,
	  std::string &filename) = 0;
	virtual bool find_next_file(std::string &filename) = 0;
};

}

#endif /*FILESYSTEM_H_*/

