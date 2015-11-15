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
		bool rslt = true;
		// dbp::compare
		if (compare()("Keep-Alive", "Set-Cookie")) {
			rslt = false;
			cerr << "compare failed (1)" << endl;
		}
		if (!compare()("aaa", "AAA")) {
			rslt = false;
			cerr << "compare failed (2)" << endl;
		}
		if (!compare()("sss", "ssS")) {
			rslt = false;
			cerr << "compare failed (3)" << endl;
		}
		if (!compare()("", "")) {
			rslt = false;
			cerr << "compare failed (4)" << endl;
		}
		if (!compare()("   ", "   ")) {
			rslt = false;
			cerr << "compare failed (5)" << endl;
		}
		// dbp::from_string / to_string
		{
			void *ptr1 = reinterpret_cast<void*>(0xb79d9f80);
			string s = to_string<void*>(ptr1);
			if (s != string("0xb79d9f80")) {
				rslt = false;
				cerr << "compare failed (6) " << ptr1 << " vs " << s << endl;
			}
		}
		{
			void *ptr1 = from_string<void*>("0xb79d9f80");
			if (ptr1 != reinterpret_cast<void*>(0xb79d9f80)) {
				rslt = false;
				cerr << "compare failed (7) " << ptr1 << " vs " << "0xb79d9f80" << endl;
			}
		}
		return rslt ? 0 : -1;
	};
	// static method to create an instance of the our application
	static application& get_instance() {
		return test().app;
	};
private:
	// the link to the console application class
	application &app;
};

IMPLEMENT_APP(test::get_instance());

