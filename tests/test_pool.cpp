#include <string>
#include <unistd.h>

#include <dcl/dclbase.h>

using namespace std;
using namespace dbp;

#define THREAD_COUNT 200
#define MAX_POOL_SIZE 5

pool<string> _pool;
pool<string> _pool_l(auto_ptr<pool_size_policy>(new pool_size_limited_policy(MAX_POOL_SIZE)));

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		bool rslt = false;
		// 1. test pool without id
		{
			pool<string> p;
			{
				pool_ptr<string> s = p.acquire();
				(*s) = "Test";
			}
			{
				pool_ptr<string> s = p.acquire();
				rslt = (*s) == string("Test");
			}
		}
		// 2. test pool with id
		{
			pool<string> p;
			{
				pool_ptr<string> s1 = p.acquire("key1");
				(*s1) = "Test1";
				pool_ptr<string> s2 = p.acquire("key2");
				(*s2) = "Test2";
				pool_ptr<string> s3 = p.acquire("");
				(*s3) = "Test3";
			}
			{
				pool_ptr<string> s1 = p.acquire("key1");
				rslt = rslt && (*s1) == string("Test1");
				pool_ptr<string> s2 = p.acquire("key2");
				rslt = rslt && (*s2) == string("Test2");
				pool_ptr<string> s3 = p.acquire("");
				rslt = rslt && (*s3) == string("Test3");
			}
			rslt = rslt && (p.size() == 3);
		}
		// 3. Test multithread
		// Create threads
		for (int i = 0; i < THREAD_COUNT - 1; i++) {
			thread *t = new thread();
			t->on_execute(create_delegate(this, &test::do_execute));
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
		rslt = rslt && (_pool.in_use() == 0);
		pool_ptr<string> s = _pool.acquire();
		rslt = (_pool.in_use() == 1) && (_pool.size() <= THREAD_COUNT) &&
		  rslt && (*s == "Test");

		// 4. Test limited size pool
		threads.clear();
		for (int i = 0; i < THREAD_COUNT; i++) {
			thread *t = new thread();
			t->on_execute(create_delegate(this, &test::do_execute_limited));
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
		rslt = rslt && (_pool_l.in_use() == 0);
		pool_ptr<string> ss = _pool_l.acquire();
		rslt = rslt && (*ss == "TestLimit");
		rslt = rslt && (_pool_l.in_use() == 1);
		rslt = rslt && (_pool_l.size() == MAX_POOL_SIZE);
		return rslt ? 0 : -1;
	};
	void do_execute(thread_int&) {
		pool_ptr<string> s = _pool.acquire();
		(*s) = "Test";
	};
	void do_execute_limited(thread_int&) {
		pool_ptr<string> s = _pool_l.acquire();
		(*s) = "TestLimit";
		usleep((rand() % 100) * 1000);
	};
	application &app;
private:
	vector<thread*> threads;
};

IMPLEMENT_APP(test().app);
