/*
 * gui_application.cpp
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

#include <cstdlib>

#include <dcl/gui_application.h>
#include <dcl/strutils.h>

namespace dbp {

using namespace std;

gui_application::gui_application() {
	bool cont = false;
	if (getenv("DCL_GUI_FRONTEND")) {
		try {
			_plugin.load(getenv("DCL_GUI_FRONTEND"));
		}
		catch (dbp::exception &e) {
			cont = true;
		}
	}
	else
	if (getenv("KDE_FULL_SESSION")) {
		try {
			_plugin.load("libdclqt.so");
		}
		catch (dbp::exception &e) {
			cont = true;
		}
	}
	else
		cont = true;
	if (cont)
		_plugin.load("libdclgtk.so");
	_widget_factory = static_cast<widget_factory*>
	  (_plugin.create_object("widget_factory"));
	if (!_widget_factory)
		throw dbp::exception(_("Invalid GUI frontend library"));
	pimpl = _widget_factory->create_application();
}

gui_application::~gui_application() {
	_plugin.destroy_object(_widget_factory);
}

} // namespace
