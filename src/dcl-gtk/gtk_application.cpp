/*
 * gtk_application.cpp
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

#include <iostream>
#include <gtk/gtk.h>

#include <dcl/datetime.h>
#include <dcl/strutils.h>
#include "gtk_application.h"

namespace dbp {

using namespace std;

gtk_application::gtk_application(): app(application::instance()) {
	app.on_exception(create_delegate(this, &gtk_application::exception_handler));
	app.on_execute(create_delegate(this, &gtk_application::execute_handler));
}

void gtk_application::exception_handler(const std::exception &e) {
	GtkWidget *dialog = gtk_message_dialog_new(NULL,
	  GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
	  (format(_("Fatal error: {0}")) % e.what()).str().c_str());
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

int gtk_application::run(int argc, char *argv[]) {
	gtk_set_locale();
	gtk_init(&argc, &argv);
	return app.run(argc, argv);
}

int gtk_application::execute_handler() {
	gtk_main();
	return 0;
}

void gtk_application::gtk_logger::log(log_level::log_level level, const std::string &message) {
	cout << "[" << datetime().now().str() << "] " << message << endl << flush;
}

} // namespace
