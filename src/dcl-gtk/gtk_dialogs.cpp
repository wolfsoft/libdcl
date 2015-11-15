/*
 * gtk_dialogs.cpp
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

#include <dcl/exception.h>
#include <dcl/strutils.h>

#include "gtk_dialogs.h"

namespace dbp {

void gtk_message_box::init(const std::string &title, const std::string &message,
  message_type::message_type type, message_buttons::message_buttons buttons) {
	switch (type) {
		case message_type::info: mt = GTK_MESSAGE_INFO; break;
		case message_type::warning: mt = GTK_MESSAGE_WARNING; break;
		case message_type::question: mt = GTK_MESSAGE_QUESTION; break;
		case message_type::error: mt = GTK_MESSAGE_ERROR; break;
	}
	switch (buttons) {
		case message_buttons::ok: bt = GTK_BUTTONS_OK; break;
		case message_buttons::close: bt = GTK_BUTTONS_CLOSE; break;
		case message_buttons::cancel: bt = GTK_BUTTONS_CANCEL; break;
		case message_buttons::yes_no: bt = GTK_BUTTONS_YES_NO; break;
		case message_buttons::ok_cancel: bt = GTK_BUTTONS_OK_CANCEL; break;
		case message_buttons::yes_no_cancel: bt = GTK_BUTTONS_NONE; break;
	}
	_title = title;
	_message = message;
}

message_response::message_response gtk_message_box::execute() {
	GtkWidget *_dialog;
	const char *title;
	if (_title.length() != 0)
		title = _title.c_str();
	else
		title = _message.c_str();
	// create the gtk_message_dialog object
	_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, mt, bt,
	  title);
	if (!_dialog)
		throw dbp::exception(
		  _("Can't initialize the GtkMessageDialog"));
	// setup dialog message body
	if (!_title.empty())
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(_dialog),
		  _message.c_str());
	// setup additional yes-no-cancel buttons if needed
	if (bt == GTK_BUTTONS_NONE) {
		gtk_dialog_add_button(GTK_DIALOG(_dialog), GTK_STOCK_CANCEL,
		  GTK_RESPONSE_CANCEL);
		gtk_dialog_add_button(GTK_DIALOG(_dialog), GTK_STOCK_NO,
		  GTK_RESPONSE_NO);
		gtk_dialog_add_button(GTK_DIALOG(_dialog), GTK_STOCK_YES,
		  GTK_RESPONSE_YES);
	}
	// run dialog
	message_response::message_response rslt = message_response::none;
	switch (gtk_dialog_run(GTK_DIALOG(_dialog))) {
		case GTK_RESPONSE_OK: rslt = message_response::ok; break;
		case GTK_RESPONSE_CANCEL: rslt = message_response::cancel; break;
		case GTK_RESPONSE_CLOSE: rslt = message_response::close; break;
		case GTK_RESPONSE_YES: rslt = message_response::yes; break;
		case GTK_RESPONSE_NO: rslt = message_response::no; break;
		case GTK_RESPONSE_APPLY: rslt = message_response::ok; break;
		case GTK_RESPONSE_HELP: rslt = message_response::help; break;
	};
	gtk_widget_destroy(_dialog);
	return rslt;
}

} //namespace
