/*
 * widget_factory.h
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

#ifndef _WIDGET_FACTORY_H_
#define _WIDGET_FACTORY_H_

#include <memory>

#include <dcl/dialogs.h>
#include <dcl/plugin.h>

namespace dbp {

class application_int;

//!	GUI widgets factory
/*!
	This is a special class to create GUI widgets. You can obtain the
	reference to this class via gui_application::factory() method and
	then create any widget supported.
*/
class widget_factory: public disposable {
public:
	typedef std::auto_ptr<application_int> gui_application_ptr;
	//! Create gui_application object
	virtual gui_application_ptr create_application() = 0;
	typedef std::auto_ptr<message_box> message_box_ptr;
	//! Create message_box object
	virtual message_box_ptr create_message_box(
	  const std::string &title, const std::string &message,
	  message_type::message_type type = message_type::error,
	  message_buttons::message_buttons buttons = message_buttons::ok) = 0;
};

} //namespace

#endif /*_WIDGET_FACTORY_H_*/
