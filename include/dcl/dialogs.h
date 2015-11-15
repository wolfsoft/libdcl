/*
 * dialogs.h
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
 
 
#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#include <string>

namespace dbp {

//! dbp::message_box dialog message types
namespace message_type {
	enum message_type { info, warning, question, error };
}

//! dbp::message_box dialog button types
namespace message_buttons {
	enum message_buttons {
		ok, close, cancel, yes_no, ok_cancel, yes_no_cancel };
}

//! dbp::message_box dialog execution result (button pressed)
namespace message_response {
	enum message_response { none, ok, close, cancel, yes, no, help };
}

//! Message box dialog
/*!
	Prompts the user with system message box and waits for user response.
	
	You can create the message_box instance via widget_factory class.
*/
class message_box {
public:
	//! Initialize the dialog.
	/*!
		Initializes the dialog with the information passed.
		
		\param title the dialog caption.
		\param message the dialog message text.
		\param type the message type.
		\param buttons dialog buttons.
	*/
	virtual void init(const std::string &title, const std::string &message,
	  message_type::message_type type = message_type::error,
	  message_buttons::message_buttons buttons = message_buttons::ok) = 0;
	//! Execute the dialog
	/*!
		\return the execution result (button pressed).
	*/
	virtual message_response::message_response execute() = 0;
};

} //namespace

#endif /*_DIALOGS_H_*/
