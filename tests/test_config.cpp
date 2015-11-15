#include <string>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;

int main() {
	app_config c;
	c.load_from_file("test.conf");
	string s("");
	s += c.value<string>("test", "string_value", "FAIL");
	s += c.value<string>("test", "string_value_2", "FAIL_2");
	s += c.value<string>("test", "string_value_3", "Test");
	s += c.value<string>("test_2", "string_value", "FAIL_3");
	const app_config::strindex &i = c.section("test");
	if ((i.size() == 7) &&
	  (s == "TestTestTestTest") &&
	  (c.value<double>("test", "digital_value", 1.0) * 50 ==
	  c.value<double>("test", "digital_value_2", 1.0)) &&
	  (c.value<bool>("test", "boolean_value", false) ==
	  c.value<bool>("test", "boolean_value_2", true)))
		return 0;
	else
		return -1;
}
