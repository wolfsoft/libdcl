#include <cstring>
#include <string>
#include <dcl/plugin.h>
#include "test_plugin.h"

using namespace std;

using namespace dbp;

class test_object_1 : public test_object {
public:
	virtual std::string test() { return "OK"; } ;
};

class test_object_2 : public test_object {
private:
	int a;
	int b;
	string c;
public:
	test_object_2(int arg1, int arg2, const string &arg3): a(arg1), b(arg2),
	  c(arg3) {};
	virtual std::string test() {
		if ((a * 10 == b) && (c == "test"))
			return "OK";
		else
			return "FAIL";
	}
};

extern "C" {
	void init(void*) { };
	
	void* create_object(const char *object_name) {
		if (strcmp(object_name, "test_object_1") == 0)
			return new test_object_1;
		else
		if (strcmp(object_name, "test_object_2") == 0)
			return new test_object_2(1, 10, "test");
		else
			return NULL;
	};
	
	void destroy_object(disposable *obj) {
		delete obj;
	};

	void finalize() { };
}
