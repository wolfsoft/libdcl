/*
 * i18n.cpp
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

#include <dcl/i18n.h>
#include <dcl/filefs.h>

#ifdef ENABLE_NLS
#include "gettext.h"
#endif

#include <locale.h>

namespace dbp {

using namespace std;

void i18n::register_catalog(const std::string &catalog) {
#ifdef ENABLE_NLS
	// setup the default locale
	setlocale(LC_ALL, "");
	filefs fs;
	bindtextdomain(catalog.c_str(), fs.get_locale_dir().c_str());
#endif
}

void i18n::register_catalog_at(const std::string &catalog,
  const std::string &path) {
#ifdef ENABLE_NLS
	// setup the default locale
	setlocale(LC_ALL, "");
	bindtextdomain(catalog.c_str(), path.c_str());
#endif
}

std::string i18n::translate(const std::string &catalog,
  const std::string &str) {
#ifdef ENABLE_NLS
	return dgettext(catalog.c_str(), str.c_str());
#else
	return str;
#endif
}

void i18n::locale(const std::string &name) {
#ifdef _WIN32
#else
	locale_t loc;
	loc = newlocale(LC_ALL_MASK, name.c_str(), NULL);
	uselocale(loc);
	freelocale(loc);
#endif
}

const std::string i18n::locale() {
#ifdef _WIN32
#else

#endif
}

} //namespace
