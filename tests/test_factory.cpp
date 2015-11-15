#include <dcl/factory.h>

using namespace std;
using namespace dbp;

class papa {
public:
	virtual int foo() = 0;
};

class son_one: public papa {
public:
	virtual int foo() {
		return 1;
	};
};

class son_two: public papa {
public:
	virtual int foo() {
		return 2;
	};
};

class first_son_child: public son_one {
public:
	virtual int foo() {
		return 4;
	};
};

int main() {
	factory<papa> f;
	f.register_class<son_one>("son_one");
	f.register_class<son_two>("son_two");
	f.register_class<first_son_child>("first_son_child");
	papa *p1 = f.create("son_one");
	papa *p2 = f.create("son_two");
	papa *p3 = f.create("first_son_child");
	return 7 - (p3->foo() + p2->foo() + p1->foo());
}

