/*
 * Simple web server example application.
 *
 * This example shows the how to write simple web server by using of
 * two DCL classes: daemon_application and web_server.
 *
 * This code is under public domain.
*/

#ifndef WWW_SERVER_H_
#define WWW_SERVER_H_

// include all classes from dclbase and dclnet libraries
#include <dcl/dclbase.h>
#include <dcl/dclnet.h>

class www_server {
public:
	www_server();
	std::string interfaces;
	std::string root_dir;
	void run();
	void stop();
private:
	dbp::http_server srv;
	dbp::mimetype mime;
	dbp::socket* on_create_io_handler(const dbp::socket&, const dbp::socket&);
	dbp::http_response on_request(const dbp::http_request&);
	void on_exception(const dbp::exception &e);
};

#endif /*WWW_SERVER_H_*/
