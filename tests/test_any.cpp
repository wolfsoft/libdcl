#include <numeric>
#include <string>
#include <vector>
#include <iostream>

#include <dcl/application.h>
#include <dcl/any.h>

using namespace std;
using namespace dbp;

class test: public application {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		any i1 = 1;
		any i2 = 2;
		any f1 = 3.0;
		any f2 = 3.4;
		any b1 = true;
		any s1 = string("test");
		vector<any> v;
		v.push_back(i1);
		v.push_back(f1);
		v.push_back(b1);
		v.push_back(s1);
		any i3 = 1.9;
		i3.cast<int>();
		i3.cast<int>();
		i3.cast<int>();
		i3.clear();
		i3 = 0.1;
		any r1 = (i1.cast<int>() + i2.cast<double>() + f2.cast<double>() + b1.cast<int>());
		any r2 = string("7.4");
		if (r1 == r2)
			return (s1 == string("test")) ? i3.cast<int>() : -1;
		else
			return -1;
	}
	// the reference to the console application class
	application &app;
};

IMPLEMENT_APP(test().app);

