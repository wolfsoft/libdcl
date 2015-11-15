#include <string>
#include <unistd.h>

#include <dcl/dclbase.h>
#include <dcl/dclodbc.h>

using namespace std;
using namespace dbp;
using namespace dbp::odbc;

#define THREAD_COUNT 500
#define MAX_POOL_SIZE 5
#define dsn string("test")
#define user string("dennis")
#define password string("sql")

class database_pool: public pool<connection>, public singleton<database_pool> {
	friend class singleton<database_pool>;
private:
	database_pool(): pool<connection>() { };
	database_pool(std::auto_ptr<pool_size_policy> policy): pool<connection>(policy) { };
};


class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	};
	int on_execute() {
		bool rslt = false;

		auto_ptr<pool_size_policy> pool_size;
		pool_size.reset(new pool_size_unlimited_policy());
		database_pool &pool = database_pool::instance(pool_size);

		pool_ptr<connection> db = pool.acquire(dsn + user);
		if (!db->is_open())
			try {
				db->open(dsn, user, password);
			}
			catch (connection_exception &e) {
				cerr << e.what() << endl << "test is skipped" << endl;
				return 0;
			}

		// 1.
		// the connections count should reach current_connections + MAX_POOL_SIZE
		int cur_conn = get_connections_number(*db);
		{
			pool_ptr<connection> carr[MAX_POOL_SIZE];
			for (int i = 0; i < MAX_POOL_SIZE; i++) {
				pool_ptr<connection> db = pool.acquire(dsn + user);
				if (!db->is_open())
					db->open(dsn, user, password);
				carr[i] = db;
			}
		}
		// the unused connections should bring back to the pool, so the
		// total number of connections should still be current_connections + MAX_POOL_SIZE
		{
			pool_ptr<connection> carr[MAX_POOL_SIZE];
			for (int i = 0; i < MAX_POOL_SIZE; i++) {
				pool_ptr<connection> db = pool.acquire(dsn + user);
				if (!db->is_open())
					db->open(dsn, user, password);
				carr[i] = db;
			}
		}
		rslt = get_connections_number(*db) == MAX_POOL_SIZE + cur_conn;

		if (!rslt)
			cout << "test #1 failed, " << get_connections_number(*db) << " connections" << endl;

		// 2.
		// the connections count should reach current_connections + MAX_POOL_SIZE
		cur_conn = get_connections_number(*db);
		{
			pool_ptr<connection> carr[MAX_POOL_SIZE];
			for (int i = 0; i < MAX_POOL_SIZE; i++) {
				carr[i] = pool.acquire(dsn + user);
				if (!carr[i]->is_open())
					carr[i]->open(dsn, user, password);
			}
			// connections should be reused, so the connections count
			// should still reach current_connections + MAX_POOL_SIZE
			for (int i = 0; i < MAX_POOL_SIZE; i++) {
				carr[i] = pool.acquire(dsn + user);
				if (!carr[i]->is_open())
					carr[i]->open(dsn, user, password);
			}
		}

		rslt = rslt && get_connections_number(*db) == MAX_POOL_SIZE + cur_conn;

		if (!rslt)
			cout << "test #2 failed, " << get_connections_number(*db) << " connections" << endl;

		return rslt ? 0 : 1;
	}
	
	int get_connections_number(connection &db) {
		query q(db);
		query::fields f = q.execute("SELECT count(*) FROM pg_stat_activity;");
		q.next();
		return from_string<int>(f.at(0).get_value());
	}
	application &app;
};

IMPLEMENT_APP(test().app);
