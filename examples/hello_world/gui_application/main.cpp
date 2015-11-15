/*
 * Classic "Hello, world!" example application with graphical user interface.
 *
 * This example shows how to write "skeleton" of the application with
 * DCL framework using dbp::application class.
 *
 * As the result, the simple GUI application will be produced.
 */

#include <iostream>
#include <string>

// include all classes from dclbase library
#include <dcl/dclbase.h>
#include <dcl/gui_application.h>

// use the DCL default namespace by default
using namespace dbp;

// declare our own application class
class hello_world_app {
public:
	// the constructor of the hello_world_app class initializes the
	// console application by obtaining a link to the dbp::application class
	// via its instance() method call.
	hello_world_app(): app(gui_application::instance()) {
		// set the application name
		app.set_name(_("Hello World GUI Application"));
		// set the application description
		app.set_description(_("Prints a classic greeting"));
		// register the execute event handler
		app.on_execute(create_delegate(this, &hello_world_app::on_execute));
	}
	// the reference to the console application class
	gui_application &app;
private:
	// execute event handler
	int on_execute() {
		widget_factory::message_box_ptr m =
		  app.factory().create_message_box(("Hello"), _("Hello, world!"),
		  message_type::info);
		return m->execute();
	};
};

// initialize and run the application via provided macros
IMPLEMENT_APP(hello_world_app().app);
