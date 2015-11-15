#include <string>
#include <iostream>

#include <dcl/dclbase.h>
#include <dcl/dclnet.h>

#ifdef _WIN32
#include <windows.h>
void sleep(int t) {
	Sleep(t * 1000);
}
#endif

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
		srv.on_request(create_delegate(this, &test::on_request));
		srv.on_connect(create_delegate(this, &test::on_connect));
		srv.on_disconnect(create_delegate(this, &test::on_disconnect));
		srv.on_exception(create_delegate(this, &test::on_exception));
		srv.on_create_io_handler(create_delegate(this,
		  &test::on_create_io_handler));
	}
	// the link to the console application class
	application &app;
private:
	http_server srv;
	int on_execute() {
/*		srv.start(":8000;localhost:8443");
		//TODO make the tests
		sleep(500);
		srv.stop();*/
		return 0;
	}
	dbp::socket* on_create_io_handler(const dbp::socket &ls,
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
		return new dbp::socket(ws);
	}
	void on_exception(const dbp::exception &e) {
		cout << "exception: " << e.what() << endl;
	}
	void on_connect(const dbp::socket &s, std::istream&, std::ostream&) {
		cout << "connecting from: " << s.address() << endl;
	}
	void on_disconnect(dbp::socket &s) {
		cout << "disconnecting from: " << s.address() << endl;
	}
	http_response on_request(const http_request &req) {
		http_response resp;
		resp.set_header("Server", "dbPager Classes Library");
		resp.set_header("Connection", "close");
		resp.set_content("Om namah shivaya");
		return resp;
	}
};

IMPLEMENT_APP(test().app);

