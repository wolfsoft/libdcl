/*
 * app_config.h
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

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#include <map>

#include <dcl/exception.h>

namespace dbp {

//!	Configuration file exception class
/*!
	The exception class is used to signal about an error when loading or
	parsing the configuration file.
*/
class app_config_exception: public dbp::exception {
public:
	//! Constructor
	app_config_exception(const std::string &msg): exception(msg) { }
};

//!	Configuration file parser class
/*!
	This class loads and parses configuration file in a common UNIX
	configuration file format, containing sections, parameters,
	its values and includes, like this:
	
	\code
[section]
option1 = value
option2 = yes
# comment
; comment
[another section]
option1 = 34
@include = other/file.conf
# and so on...
	\endcode
*/
class app_config {
public:
	typedef std::map<std::string, std::string > strindex;
	typedef std::map<std::string, strindex > sections;
	//!	Clear currently loaded options
	void clear();
	//!	Load the configuration parameters from the given file
	/*!
		Clears currently loaded options and loads the new options
		from the configuration file.

		\param name a name of the file to load.
	*/
	void load_from_file(const std::string &name);
	//!	Get the option value
	/*!
		This function returns the option value, or, if the option is not
		defined, the default value provided.
		
		\param section a name of the option section, without brackets.
		\param name a name of the option.
		\param def_value an option default value.
		\return option value or default value, if the option is not defined.
	*/
	template <class T>
	T value(const std::string &section,
	  const std::string &name, T def_value) const;
	//!	Get the options section
	/*!
		This function returns the section content as a whole.

		\param section_name a name of the option section, without brackets.
		\return a list of all options of the given section.
	*/
	const strindex& section(const std::string &section_name) const;
	//! Get the sections iterator
	/*!
		Use get_sections() to iterate through all loaded sections to obtain
		all section names, for example.
	*/
	sections::const_iterator get_sections();
	//! Get the tail of the sections iterator
	/*!
		When iterating through sections, use the end() method for stopping
		the iteration, i.e.
		\code
for (app_config::sections::const_iterator i = cfg.get_sections();
  i != cfg.end(); ++i) {
	// (std::string) i->first contains the section name
	// (strindex) i->second contains the section options
}
		\endcode
	*/
	sections::const_iterator end();
private:
	strindex _empty;
	sections options;
	void do_load(const std::string &name);
};

template<>
const std::string& app_config::value<const std::string&>(
  const std::string &section, const std::string &name,
  const std::string &def_value) const;

template<>
std::string app_config::value<std::string>(const std::string &section,
  const std::string &name, std::string def_value) const;

template<>
const char* app_config::value<const char*>(const std::string &section,
  const std::string &name, const char *def_value) const;

template<>
int app_config::value<int>(const std::string &section,
  const std::string &name, int def_value) const;

template<>
double app_config::value<double>(const std::string &section,
  const std::string &name, double def_value) const;

template<>
bool app_config::value<bool>(const std::string &section,
  const std::string &name, bool def_value) const;

} /*namespace*/

#endif /*_APP_CONFIG_H_*/

