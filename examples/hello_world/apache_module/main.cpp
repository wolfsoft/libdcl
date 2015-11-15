/*
 * Classic "Hello, world!" example application (Apache module).
 *
 * This example shows how to write "skeleton" of the application with
 * DCL framework using dbp::apache_application class.
 *
 * As the result, the simple apache module will be produced.
 */

#include <iostream>
#include <string>

// include all classes from dclbase library
#include <dcl/dclbase.h>
#include <dcl/apache_application.h>

// use the DCL default namespace by default
using namespace dbp;

// declare our own application class
class hello_world_app {
public:
	// the constructor of the hello_world_app class initializes the
	// console application by obtaining a link to the dbp::application class
	// via its instance() method call.
	hello_world_app() {
		// register the execute event handler
		app.on_handle_request(create_delegate(this,
		  &hello_world_app::process_request));
	}
	apache_application& application_impl() {
		return app;
	};
private:
	// the apache application
	apache_application app;
	// execute event handler
	http_response process_request(const http_request &req) {
		// initialize the response
		http_response resp;
		// we are only support GET method in this demo
		if (req.get_method() == http_method::get) {
			// initialize the response with greeting message
			resp.set_content_type("text/plain");
			resp.set_content("Hello, world!");
		} else {
			// send the response with "405 Method Not Allowed" status code
			resp.set_status(http_error::method_not_allowed);
			resp.set_allow(http_method::get);
		}
		// send the response to the client
		return resp;
	};
};

// initialize and run the application via provided macros
IMPLEMENT_APP(hello_world_module, hello_world_app);

