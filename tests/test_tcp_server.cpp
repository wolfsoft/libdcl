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
		srv.on_connect(create_delegate(this, &test::on_connect));
		srv.on_disconnect(create_delegate(this, &test::on_disconnect));
		srv.on_process_data(create_delegate(this, &test::on_process_data));
	};
	// the link to the console application class
	application &app;
private:
	tcp_server srv;
	int on_execute() {
/*		srv.start(":7777");
		//TODO make the tests
		sleep(500);
		srv.stop();*/
		return 0;
	};
	void on_connect(const dbp::socket &s, std::istream&, std::ostream&) {
		cout << "connected from " << s.address() << ":" << s.port() << endl;
	}
	void on_disconnect(dbp::socket &s) {
		cout << s.address() << ":" << s.port() << " disconnected." << endl;
	}
	bool on_process_data(const dbp::socket&, std::istream &in, std::ostream &out) {
		if (in.bad()) {
			cout << "The reading error occured." << endl;
			return false;
		}
		// read full line into the buffer
		string buf;
		int pos = in.tellg();
		getline(in, buf);
		if (in.eof()) {
			in.seekg(pos);
			return true;
		}
		// line readed
		if (!buf.empty()) {
            if (trim()(buf) == string("quit")) {
                out << "good bye!" << endl;
                return false;
            }
			cout << buf << endl;
			out << "Om namah shivaya, " << buf << endl;
		}
		return true;
	}
};

IMPLEMENT_APP(test().app);

