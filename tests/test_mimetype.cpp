#include <string>
#include <iostream>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		mimetype mime;
		return (mime("./test.conf") == "text/plain") ? 0 : -1;
	};
	// the reference to the console application class
	application &app;
};

IMPLEMENT_APP(test().app);

