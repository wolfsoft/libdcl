#include <iostream>
#include <sstream>
#include <string>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		stringstream in, out;
		process p(in, out, out);
		#ifdef _WIN32
		p.execute("CMD.EXE", "/C echo test");
		return (out.str() == "test\r\n") ? 0 : 1;
		#else
		p.execute("/bin/sh", "-c \"echo test\"");
		return (out.str() == "test\n") ? 0 : 1;
		#endif
	};
	// the reference to the console application class
	application &app;
};

IMPLEMENT_APP(test().app);
