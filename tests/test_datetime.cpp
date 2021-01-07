#include <string>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	int on_execute() {
		setlocale(LC_ALL, "C");
		bool rslt = true;
		datetime d1;
		d1.year(2000).month(9).day(1).hour(2).minute(3).second(4);
		datetime d2("Fri, 01 Sep 2000 02:03:04 GMT");
		rslt = rslt && (d1 == d2);
		return rslt ? 0 : -1;
	}
	application &app;
};

IMPLEMENT_APP(test().app);
