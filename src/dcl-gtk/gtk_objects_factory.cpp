/*
 * gtk_objects_factory.cpp
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

#include <cstring> 

#include <dcl/plugin.h>
#include <dcl/widget_factory.h>

#include "gtk_application.h"
#include "gtk_dialogs.h"

namespace dbp {

using namespace std;

class gtk_widget_factory: public widget_factory {
	virtual gui_application_ptr create_application() {
		return gui_application_ptr(new gtk_application());
	};
	virtual message_box_ptr create_message_box(
	  const std::string &title, const std::string &message,
	  message_type::message_type type = message_type::error,
	  message_buttons::message_buttons buttons = message_buttons::ok) {
		message_box_ptr rslt(new gtk_message_box());
		rslt->init(title, message, type, buttons);
		return rslt;
	};
};

// Export two functions for creating/destroying object, as required by
// dbp::plugin class

extern "C" {

void init(void*) { };

disposable* create_object(const char *object_name) {
	if (strcmp(object_name, "widget_factory") == 0)
		return new gtk_widget_factory();
	else
		return NULL;
};

void destroy_object(disposable *object) {
	delete object;
};

void finalize() { };

} // extern

} // namespace
