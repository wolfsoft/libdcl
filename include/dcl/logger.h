/*
 * logger.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2015 Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>

namespace dbp {

namespace log_level {
	enum log_level {
		debug = 7,
		info = 6,
		notice = 5,
		warning = 4,
		error = 3,
		critical = 2,
		alert = 1,
		emergency = 0
	};
}

//! Base logger class
/*!
	Each concrete implementation should override log() method.
*/
class logger {
public:
	virtual ~logger() { };

	virtual void log(log_level::log_level level, const std::string &message) = 0;

	void debug(const std::string &message) {
		log(log_level::debug, message);
	};
	void info(const std::string &message) {
		log(log_level::info, message);
	};
	void notice(const std::string &message) {
		log(log_level::notice, message);
	};
	void warning(const std::string &message) {
		log(log_level::warning, message);
	};
	void error(const std::string &message) {
		log(log_level::error, message);
	};
	void critical(const std::string &message) {
		log(log_level::critical, message);
	};
	void alert(const std::string &message) {
		log(log_level::alert, message);
	};
	void emergency(const std::string &message) {
		log(log_level::emergency, message);
	};
};

}

#endif /*_LOGGER_H_*/

