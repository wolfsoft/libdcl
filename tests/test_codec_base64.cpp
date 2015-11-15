#include <string>
#include <sstream>
#include <iostream>

#include <dcl/application.h>
#include <dcl/codec_base64.h>

using namespace std;
using namespace dbp;
using namespace dbp::codec;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	int on_execute() {
		stringstream s("Test String!");
		stringstream r;
		codec_base64 c;
		c.encode(s, r);
		r.seekg(0);
		stringstream s1;
		c.decode(r, s1);
		if (s1.str() == "Test String!")
			return 0;
		else
			return -1;
	}
	application &app;
};

IMPLEMENT_APP(test().app);