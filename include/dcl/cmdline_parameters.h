/*
 * cmdline_parameters.h
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

#ifndef _CMDLINE_PARAMETERS_H_
#define _CMDLINE_PARAMETERS_H_

#include <vector>
#include <string>

#include <dcl/exception.h>

namespace dbp {

//!	Command line parameter exception
/*!
	This exception class is used to signal about error on command line
	parameters parsing.
*/
class cmdline_parameters_exception: public exception {
public:
	//! Constructor
	cmdline_parameters_exception(const std::string &msg): exception(msg) { }
};

//!	Command line parameter
/*!
	The command line parameter class represents parameter passed to application
	via command line.
*/
class cmdline_parameter {
public:
	//! Type of command line parameter
	enum parameter_type {
	  /*! represents a single option, with "-" or "--" prefix,
	  	  for example: '-v', '--help' */
	  OPTION,
	  /*! represents an option with value, with "-" or "--" prefix,
	  for example: '-i file.txt', '--input=file.txt'. */
	  OPTION_WITH_VALUE,
	  /*! represents the command. */
	  COMMAND,	  
	  /*! represents a filename passed to the application. */
	  FILENAME
	};
	char short_name;			/*!< Short option name. */
	std::string long_name;		/*!< Long option name. */
	std::string value_name;		/*!< Option value name. */
	std::string value;			/*!< Option value. */
	std::string description;	/*!< Option description. */
	parameter_type type; 		/*!< Parameter type. */
	//! Constructor
	cmdline_parameter(): short_name(0), type(FILENAME) { };
	//! Constructor
	explicit cmdline_parameter(char short_name,
	  const std::string &long_name, const std::string &description,
	  const std::string &value_name = "",
	  parameter_type type = OPTION): short_name(short_name),
	  long_name(long_name), value_name(value_name), description(description),
	  type(type) { };
	//! Constructor
	explicit cmdline_parameter(parameter_type type): short_name(0),
	  type(type) { };
	//! Constructor
	explicit cmdline_parameter(const std::string &description,
	  parameter_type type): short_name(0), description(description),
	  type(type) { };
	//! Equality operator
	bool operator==(const parameter_type p_type) const;
	//! Equality operator
	bool operator==(const std::string &name) const;
};


//!	Command line parser
/*!
	This class represents application command line parameter list and
	parser.
*/
class cmdline_parameters {
public:
	//! Command line parameters
	typedef std::vector<cmdline_parameter> params;
	//! Constructor
	cmdline_parameters(): _pos(0) { };
	//! Register known command line parameter
	/*
		Registers the command line parameter as known one.
		
		\param param the command line parameter
	*/
	void add(const cmdline_parameter &param);
	//! Parse command line
	/*!
		Parses given command line. After sequential calls iterates throw all
		command line parameters found.
		
		\param argc the command line parameters count.
		\param argv[] pointer to the command line.
		\param param (out) the next command line parameter found.
		\return true if one more command line parameter exist and false
		otherwise.
	*/
	bool parse(int argc, char* argv[], cmdline_parameter &param);
	//! Registered command line parameters iterator
	/*!
		Returns the iterator for the beginning of registered command line
		parameters collection.
	*/
	params::const_iterator begin() const;
	//! Registered command line parameters iterator
	/*!
		Returns the iterator for the ending of registered command line
		parameters collection.
	*/
	params::const_iterator end() const;
private:
	int _pos;
	params _params;
};

} // namespace

#endif /*_CMDLINE_PARAMETERS_H_*/

