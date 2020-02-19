#include <string>
#include <iostream>

#include <dcl/application.h>
#include <dcl/url.h>

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	int on_execute() {
		{
			url u("http://clickhouse:8123/?database=bg");
			if (!do_test(u.str() == string("http://clickhouse:8123/?database=bg"), "localhost failed, " + u.str())) return -1;
		}
		url u1 = "http://www.test.com:8080/~path/script?param=value#here";
		url u2("http://www.test.com/~path/?param=value2");
		if (!do_test((u1.scheme == u2.scheme), "scheme failed")) return -1;
		if (!do_test((u1.host == u2.host), "host failed")) return -1;
		if (!do_test((u1.path == u2.path + "script"), "path failed")) return -1;
		if (!do_test((u2.str() == "http://www.test.com/~path/?param=value2"),
		  "url decode failed " + u2.str())) return -1;
		if (!do_test((u2.path == "/~path/"), "~path failed " + u2.path)) return -1;
		if (!do_test((u2.encode("\xD5\xC3\xC9") ==
		  "%D5%C3%C9"), "encode failed " + u2.encode("\xD5\xC3\xC9"))) return -1;
		if (!do_test((u2.decode("%D5%C3%C9") ==
		  "\xD5\xC3\xC9"), "decode failed " + u2.decode("%D5%C3%C9"))) return -1;
		if (!do_test((u1.decode(u2.encode("This!@#$%^&*()%01%02is a test")) ==
		  "This!@#$%^&*()%01%02is a test"), "encode/decode failed")) return -1;
		if (!do_test((u1.port - u2.port == 8000), "port failed")) return -1;
		url u4;
		u4.scheme = "file";
		u4.path = "/some/file.txt";
		url u3;
		u3.parse("../../.././file.txt");
		u3.scheme = "file";
		u3.normalize("/some/path/to/set/");
		if (!do_test((u4.str() == "file:///some/file.txt"), "str() failed")) return -1;
		if (!do_test((u3.path == u4.path), "local path failed " + u3.path + " " + u4.path)) return -1;
		// windows path
		url u5 = "C:/Program Files/dbPager Server/bin/libdbp_isapi-3.dll";
		u5.scheme = "file";
		if (!do_test((u5.str() == "file:///C:/Program Files/dbPager Server/bin/"
		  "libdbp_isapi-3.dll"), "windows path failed " + u5.str())) return -1;
		url u6 = "file://www.test.com/~path/";
		if (!do_test((u6.host == "www.test.com" && u6.path == "/~path/"),
		  "file schema failed " + u6.str())) return -1;

		{
			url test_url = "https://google.com";
			if (!do_test((test_url.str() == "https://google.com/"),
			  "no root path test failed " + test_url.str())) return -1;

		}
		return 0;
	}
	application &app;
private:
	inline bool do_test(bool rslt, const std::string &message) {
		if (!rslt)
			cout << message << endl;
		return rslt;
	};
};

IMPLEMENT_APP(test().app);
