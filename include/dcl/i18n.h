/*
 * i18n.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2009 Dennis Prochko <wolfsoft@mail.ru>
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

#ifndef _I18N_H_
#define _I18N_H_

#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace dbp {

//!	Shortcut for i18n::translate()
#define _(str) std::string(dbp::i18n::translate(PACKAGE, str))

//! Internationalization helper class
/*!
	This class is a wrapper around GNU gettext subsystem that provides
	internationalization features to your application.
	
	Use the shortcut _("string") to avoid use this class directly.
*/
class i18n {
public:
	//! Load the translation catalog
	/*!
		\param catalog the name of the resources catalog (your application
		name).
	*/
	static void register_catalog(const std::string &catalog);
	//! Load the translation catalog at the specified directory
	/*!
		\param catalog the name of the resources catalog (your application
		name).
		\param path the path to the catalog directory.
	*/
	static void register_catalog_at(const std::string &catalog,
	  const std::string &path);
	//! Translate the given english string to the current locale language
	/*!
		\param catalog the name of the resources catalog (your application
		name).
		\param str the string to translate.
		\return the translated string.
	*/
	static std::string translate(const std::string &catalog,
	  const std::string &str);
	//! Set the locale for the current thread
	/*!
	*/
	static void locale(const std::string &name);
	//! Retrieve the current locale
	/*!
	*/
	static const std::string locale();
};

} // namespace

#endif /*_I18N_H_*/

