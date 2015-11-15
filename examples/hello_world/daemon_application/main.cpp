/*
 * Classic "Hello, world!" example console application.
 *
 * This example shows how to write "skeleton" of the application with
 * DCL framework using application class.
 * As the result, the simple console application will be produced.
 *
 * This code is under public domain.
 */

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// include all classes from dclbase library
#include <dcl/dclbase.h>
#include <dcl/daemon_application.h>

// use the DCL default namespace by default
using namespace dbp;

// declare our own application class
class hello_world_app {
public:
	// the constructor of the hello_world_app class initializes the
	// console application by obtaining a link to the dbp::application class
	// via its instance() method call.
	hello_world_app(): app(daemon_application::instance()),
	  is_terminated(false) {
		// set the application name
		app.set_name(_("Hello World Daemon"));
		// set the application description
		app.set_description(_("Prints a standard welcome greeting in "
		  "a standard system log every 10 seconds"));
		// register the custom command line parameter handler
		app.register_cmdline_parameter(cmdline_parameter('v', "version",
		  _("show the application version"), "", cmdline_parameter::OPTION),
		  create_delegate(this, &hello_world_app::on_version_parameter));
		// register the execute event handler
		app.on_execute(create_delegate(this, &hello_world_app::on_execute));
		// register the terminate event handler
		app.on_terminate(create_delegate(this, &hello_world_app::on_terminate));
	}
	// the reference to the daemon_application class
	daemon_application &app;
private:
	bool is_terminated;
	// "-v" command line parameter handler
	bool on_version_parameter(cmdline_parameter&) {
		std::cout << _("version 1.0.0") << std::endl;
		return false;
	};
	void on_terminate() {
		is_terminated = true;
	};
	// execute event handler
	int on_execute() {
		// repeat while application is not stopped
		while (!is_terminated) {
			// write greeting to system log
			app.log("Hello, world!");
			// wait 10 secs
#ifdef _WIN32
			Sleep(10 * 1000);
#else
			sleep(10);
#endif
		}
		return 0;
	};
};

// initialize and run the application via provided macros
IMPLEMENT_APP(hello_world_app().app);
