#include <string>

#include <dcl/dclbase.h>
#include <dcl/dclnet.h>

using namespace std;
using namespace dbp;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		bool rslt = true;
		// test application/x-www-form-urlencoded
		{
			http_header h;
			h.set_header("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
			h.set_content("ascii=ascii_value&encoded=%25+value&utf=%D0%AA%D0%AC%25");
			http_content_elements e = http_content_parser(h).parse();
			// check for elements count
			rslt = rslt && e.size() == 3;
			if (!rslt) {
				cerr << "application/x-www-form-urlencoded failed; " <<
				"3 elements expected, " << e.size() << " found." << endl;
				return -1;
			}
			// check for ascii value
			rslt = rslt && e[0].name == "ascii" && e[0].value == "ascii_value";
			if (!rslt) {
				cerr << "application/x-www-form-urlencoded failed; " <<
				"ascii_value expected, " << e[0].value << " found." << endl;
				return -1;
			}
			// check for encoded value
			rslt = rslt && e[1].name == "encoded" && e[1].value == "% value";
			if (!rslt) {
				cerr << "application/x-www-form-urlencoded failed; " <<
				"'% value' expected, '" << e[1].value << "' found." << endl;
				return -1;
			}
			// check for unicode value
			rslt = rslt && e[2].name == "utf" && e[2].value == "ЪЬ%";
			if (!rslt) {
				cerr << "application/x-www-form-urlencoded failed; " <<
				"'ЪЬ%' expected, '" << e[2].value << "' found." << endl;
				return -1;
			}
		}
		// test text/plain
		{
			http_header h;
			h.set_header("Content-Type: text/plain");
			h.set_content("test %25 test");
			http_content_elements e = http_content_parser(h).parse();
			// check for elements count
			rslt = rslt && e.size() == 1;
			if (!rslt) {
				cerr << "text/plain failed; " <<
				"1 element expected, " << e.size() << " found." << endl;
				return -1;
			}
			// check for value
			rslt = rslt && e[0].name == "content" && e[0].value == "test %25 test";
			if (!rslt) {
				cerr << "text/plain failed; " <<
				"'test %25 test' expected, " << e[0].value << " found." << endl;
				return -1;
			}
		}
		// test multipart/form-data
		{
			http_header h;
			h.set_header("Content-Type: multipart/form-data; boundary=AaB03x");
			h.set_content(
				"--AaB03x\n"
				"Content-Disposition: form-data; name=\"submit-name\"\n"
				"\n"
				"Larry\n"
				"--AaB03x\n"
				"Content-Disposition: form-data; name=\"files\"\n"
				"Content-Type: multipart/mixed; boundary=BbC04y\n"
				"\n"
				"--BbC04y\n"
				"Content-Disposition: file; filename=\"file1.txt\"\n"
				"Content-Type: text/plain\n"
				"\n"
				"... contents of file1.txt ...\n"
				"--BbC04y\n"
				"Content-Disposition: file; filename=\"file2.gif\"\n"
				"Content-Type: image/gif\n"
				"Content-Transfer-Encoding: binary\n"
				"\n"
				".\x00\x01\x02...other contents of file2.gif...\n"
				"--BbC04y--\n"
				"--AaB03x--\n"
			);
			http_content_elements e = http_content_parser(h).parse();
			// check for elements count
			rslt = rslt && e.size() == 3;
			if (!rslt) {
				cerr << "multipart/form-data failed; " <<
				"3 elements expected, " << e.size() << " found." << endl;
				return -1;
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

