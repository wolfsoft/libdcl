/*
 * Simple web server example application.
 *
 * This example shows the how to write simple web server by using of
 * two DCL classes: daemon_application and web_server.
 *
 * This code is under public domain.
*/

#include <fstream>

#ifdef _WIN32
#include <windows.h>
void sleep(int t) {
	Sleep(t * 1000);
}
#endif

#include "www_server.h"

using namespace std;
using namespace dbp;

www_server::www_server():
  interfaces(":8000;:8443"),
  root_dir("/var/www") {
	srv.on_create_io_handler(create_delegate(this,
	  &www_server::on_create_io_handler));
	srv.on_exception(create_delegate(this, &www_server::on_exception));
	srv.on_request(create_delegate(this, &www_server::on_request));
}

void www_server::run() {
	srv.start(interfaces);
	while (srv.is_running()) {
		sleep(1);
	}
}

void www_server::stop() {
	srv.stop();
}

void www_server::on_exception(const dbp::exception &e) {
	//TODO write error to the system log
	throw e;
}

dbp::socket* www_server::on_create_io_handler(const dbp::socket &ls,
  const dbp::socket &ws) {
	// is it SSL connection?
	if (ls.port() == 8443) {
		ssl_socket *s = new ssl_socket(ws);
		s->load_certificates(
		  nullstr,
		  "/home/dennis/development/libdcl-0.1.0/examples/web_server/cert.pem",
		  "/home/dennis/development/libdcl-0.1.0/examples/web_server/pkey.pem",
		  nullstr);
		return s;
	}
	// no, it's simple connection type
	return new socket(ws);
}

http_response www_server::on_request(const http_request &req) {
	http_response resp;
	// we are support only HTTP GET method
	if (req.get_method() != http_method::get) {
		resp.set_status(http_error::method_not_allowed);
		resp.set_allow(http_method::get);
        resp.set_content("The POST requests are not supported yet.");
		return resp;
	}
	// obtain the name of the file requested
	url u(req.get_path_info());
	u.normalize(root_dir);
	// load file content to the server response
	try {
		ifstream in(u.path.c_str());
		// if file can't be open, throw an error
		if (!in)
			throw dbp::exception(string("File not found: ") + u.path);
		// set file content type
		resp.set_content_type(mime(u.path));
		// load file
		resp.add_content(in);
	}
	catch (dbp::exception &e) {
		// return error message
		resp.set_status(http_error::not_found);
		resp.set_content(e.what());
	}
	return resp;
}
