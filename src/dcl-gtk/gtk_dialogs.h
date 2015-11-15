/*
 * gtk_dialogs.h
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

#ifndef _GTK_DIALOGS_H_
#define _GTK_DIALOGS_H_

#include <string>
#include <gtk/gtk.h>

#include <dcl/dialogs.h>

namespace dbp {

class gtk_message_box: public message_box {
public:
	virtual void init(const std::string &title, const std::string &message,
	  message_type::message_type type = message_type::error,
	  message_buttons::message_buttons buttons = message_buttons::ok);
	virtual message_response::message_response execute();
private:
	GtkMessageType mt;
	GtkButtonsType bt;
	std::string _title, _message;
};

}

#endif /*_GTK_DIALOGS_H_*/
