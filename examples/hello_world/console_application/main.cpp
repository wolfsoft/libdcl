/*
 * Classic "Hello, world!" example console application.
 *
 * This example shows how to write "skeleton" of the application with
 * DCL framework using dbp::application class.
 *
 * As the result, the simple console application will be produced.
 */

#include <iostream>
#include <string>

// include all classes from dclbase library
#include <dcl/dclbase.h>

// use the DCL default namespace by default
using namespace dbp;

// declare our own application class
class hello_world_app {
public:
	// the constructor of the hello_world_app class initializes the
	// console application by obtaining a link to the dbp::application class
	// via its instance() method call.
	hello_world_app(): app(application::instance()) {
		// set the application name
		app.set_name(_("Hello World Application"));
		// set the application description
		app.set_description(_("Prints a classic greeting"));
		// register the custom command line parameter handler
		app.register_cmdline_parameter(cmdline_parameter('v', "version",
		  _("show the application version"), "", cmdline_parameter::OPTION),
		  create_delegate(this, &hello_world_app::on_version_parameter));
		// register the execute event handler
		app.on_execute(create_delegate(this, &hello_world_app::on_execute));
	}
	// the reference to the console application class
	application &app;
private:
	// "-v" command line parameter handler
	bool on_version_parameter(cmdline_parameter&) {
		std::cout << _("version 1.0.0") << std::endl;
		return false;
	};
	// execute event handler
	int on_execute() {
		std::cout << _("Hello, world!") << std::endl;
		return 0;
	};
};

// initialize and run the application via provided macros
IMPLEMENT_APP(hello_world_app().app);

