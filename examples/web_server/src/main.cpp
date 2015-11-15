/*
 * Simple web server example application.
 *
 * This example shows the how to write simple web server by using of
 * two DCL classes: daemon_application and web_server.
 *
 * This code is under public domain.
*/

// include all classes from dclbase library
#include <dcl/dclbase.h>
#include <dcl/daemon_application.h>

#include "www_server.h"

using namespace std;
using namespace dbp;

// our web server application class should be a daemon
class web_server_app {
public:
	// the constructor of the web_server_app class initializes the
	// console application by obtaining a link to the dbp::application class
	// via its instance() method call.
	web_server_app(): app(daemon_application::instance()),
	  is_terminated(false) {
		// set the application description
		app.set_description("Simple demo web server");
		// Registering custom application command line switches
		app.register_cmdline_parameter(cmdline_parameter('l', "listen",
		  _("network interface(s) to listen on"), _("ip address:port"),
		  cmdline_parameter::OPTION_WITH_VALUE),
		  create_delegate(this, &web_server_app::on_cmdline_parameter));
		app.register_cmdline_parameter(cmdline_parameter('r', "root",
		  _("web server root directory"), _("path"),
		  cmdline_parameter::OPTION_WITH_VALUE),
		  create_delegate(this, &web_server_app::on_cmdline_parameter));
		// register the execute event handler
		app.on_execute(create_delegate(this, &web_server_app::on_execute));
		// register the terminate event handler
		app.on_terminate(create_delegate(this, &web_server_app::on_terminate));
	}
	// the reference to the daemon application class
	daemon_application &app;
private:
	bool is_terminated;
	www_server server;
	bool on_cmdline_parameter(cmdline_parameter &param) {
		// check for known command line switches and apply passed parameters
		switch (param.short_name) {
			case 'l':
				server.interfaces = param.value;
				return true;
				break;
			case 'r':
				server.root_dir = param.value;
				return true;
				break;
			default:
				return false;
		}
	}
	void on_terminate() {
		server.stop();
	}
	int on_execute() {
		server.run();
		return 0;
	}
};

// initialize the application
IMPLEMENT_APP(web_server_app().app);

