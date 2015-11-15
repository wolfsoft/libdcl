#include <algorithm>
#include <string>
#include <vector>

#include <dcl/dclbase.h>
#include <dcl/shared_ptr.h>

using namespace std;
using namespace dbp;

static int cnt;

class test {
public:
	class tst {
	public:
		tst() { cnt++; }
		virtual ~tst() { cnt--; }
	};
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	application &app;
private:
	int on_execute() {
		cnt = 0;
		{
			std::vector<dbp::shared_ptr<tst> > v;
			v.push_back(dbp::shared_ptr<tst>(new tst()));
			v.push_back(dbp::shared_ptr<tst>(new tst()));
			v.push_back(dbp::shared_ptr<tst>(new tst()));
			v.push_back(dbp::shared_ptr<tst>(new tst()));
			sort(v.begin(), v.end());
		}
		return cnt;
	}
};

IMPLEMENT_APP(test().app);
