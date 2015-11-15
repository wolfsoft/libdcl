#include <string>
#include <sstream>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;
using namespace dbp::codec;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	int on_execute() {
		encoder_md5 md5;
		istringstream in1("");
		istringstream in2("The quick brown fox jumps over the lazy dog");
		ostringstream out("");
		md5.encode(in1, out);
		bool rslt = (out.str() == string("d41d8cd98f00b204e9800998ecf8427e"));
		out.seekp(0);
		md5.encode(in2, out);
		rslt = rslt && (out.str() == string("9e107d9d372bb6826bd81d3542a419d6"));
		return rslt ? 0 : -1;
	}
	application &app;
};

IMPLEMENT_APP(test().app);