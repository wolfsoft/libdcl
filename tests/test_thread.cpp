#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;

#define THREAD_COUNT 21

class job {
public:
	job(): app(application::instance()), counter(0) {
		app.on_execute(create_delegate(this, &job::on_execute));
	};
	int on_execute() {
		// Create threads
		for (int i = 0; i < THREAD_COUNT; i++) {
			thread *t = new thread();
			t->on_execute(create_delegate(this, &job::do_execute));
			threads.push_back(t);
			t->start();
		}
		// wait for threads job done
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			(*i)->wait_for();
		}
		// dispose threads
		for (vector<thread*>::const_iterator i = threads.begin();
		  i != threads.end(); ++i) {
			delete (*i);
		}
		// return the result
		return THREAD_COUNT - counter;
	};
	void do_execute(thread_int&) {
		vector<int> v(1024 * 100);
		generate(v.begin(), v.end(), rand);
		sort(v.begin(), v.end());
		mutex_guard m(cs);
		counter++;
	};
	// the reference to the console application class
	application &app;
private:
	mutex cs;
	volatile int counter;
	vector<thread*> threads;
};

IMPLEMENT_APP(job().app);
