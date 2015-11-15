#include <string>
#include <iostream>

#include <dcl/plugin.h>

#include "test_plugin.h"

using namespace std;
using namespace dbp;

int main() {
	string test_string("");
	{
		plugin p;
		try {
			#ifdef WIN32
			p.load(".libs/libplugin.dll");
			#else
			p.load(".libs/libplugin.so");
			#endif
			disposable *o1, *o2;
			o1 = p.create_object("test_object_1");
			o2 = p.create_object("test_object_2");
			test_string += static_cast<test_object*>(o1)->test(); // adds OK
			test_string += static_cast<test_object*>(o2)->test(); // adds OK
			p.destroy_object(o1);
			p.destroy_object(o2);
		}
		catch (plugin_exception &e) {
			cerr << e.what() << endl;
			return -1;
		}
	}
	if (test_string == "OKOK")
		return 0;
	else
		return -2;
}
