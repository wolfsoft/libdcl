#include <string>
#include <iostream>

#include <dcl/application.h>
#include <dcl/uuid.h>

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	int on_execute() {
		uuid u1;
		uuid u2;
		uuid u3;
		u1.clear();
		if (!do_test(u2 && !u1, "uuid::clear failed"))
			return -1;
		if (!do_test(u2 != u1, "uuid::operator== failed"))
			return -1;
		if (!do_test(u2 != u3, "uuid::generate() failed"))
			return -1;
		u2 = "f81d4fae-7dec-11d0-a765-00a0c91e6bf6";
		uuid u4("f81d4fae-7dec-11d0-a765-00a0c91e6bf6");
		if (!do_test(u2 == u4, "uuid::assign() failed"))
			return -1;
		if (!do_test(u4 == "f81d4fae-7dec-11d0-a765-00a0c91e6bf6", "uuid::str() failed"))
			return -1;
		return 0;
	}
	application &app;
private:
	inline bool do_test(bool rslt, const std::string &message) {
		if (!rslt)
			cout << message << endl;
		return rslt;
	}
};

IMPLEMENT_APP(test().app);