#include <string>
#include <iostream>

#include <dcl/dclbase.h>
#include <dcl/dclodbc.h>

using namespace std;
using namespace dbp;
using namespace dbp::odbc;

class test {
public:
	test(): app(application::instance()) {
		app.on_execute(create_delegate(this, &test::on_execute));
	}
	int on_execute() {
		// connect to test database if available
		connection db;
		try {
			db.open("test", "dennis", "sql");
		}
		catch (connection_exception &e) {
			cerr << e.what() << endl << "test is skipped" << endl;
			return 0;
		}
		// create test tables
		query q(db);
		try { q.execute("drop table test2"); } catch (dbp::exception &e) { };
		try { q.execute("drop table test1"); } catch (dbp::exception &e) { };
		q.execute("create table test1 (id integer not null primary key, value varchar(128))");
		q.execute("create table test2 (id integer not null primary key, "
		          "test1code int references test1(id), value numeric(15,2))");
		// populate tables with data via parameterized query
		q("insert into test1 (id, value) values (?, ?)")
		  << 1 << "test1" << query::flush()
		  << 2 << "test2" << query::flush()
		  << 3 << "test3" << query::flush()
		  << 4 << "" << query::flush();
		q("insert into test2 (id, test1code, value) values (:id, ?, :value)")
		  << 1 << 1 << 0.1 << query::flush()
		  << 2 << 2 << 0.2 << query::flush()
		  << 3 << 3 << 0.3 << query::flush();
		// try to use transactions
		db.begin_transaction();
		q << 4 << 1 << 0.2 << query::flush();
		db.rollback();
		db.begin_transaction();
		q << 5 << 1 << 0.9 << query::flush();
		db.commit();
		// some updates
		{
			query::parameters &p = q.prepare("update test2 set value = 0.15 where id = :id");
			p.at(0).value = "1";
			q.execute();
		}
		// some updates again
		{
			query::parameters &p = q.prepare("update test2 set value = 0.15 where id = :i_d");
			p.at(0).value = "1";
			q.execute();
		}
		// some fake updates
		{
			query::parameters &p = q.prepare("update test2 set value = -1 where id = :id and value = :value");
			p.at(0).value = "1";
			q.execute();
		}
		// some deletes when no row affected
		q("delete from test2 where id = :id") << -1 << query::flush();
		// some deletes with NULL param
		{
			query::parameters &p = q.prepare("delete from test2 where id = :id");
			q.execute();
			while (q.next()) { }
		}
		// take the test result
		query::fields f =
		  q.execute("select sum(t2.value), t1.value from test2 t2, test1 t1 "
		    "where t1.id = t2.test1code group by t1.value");
		stringstream s;
		for (query::fields::iterator i = f.begin(); i != f.end(); ++i) {
			s << "'" << i->name << "'" << " ";
		}
		double qr;
		string val;
		while (q.next()) {
			q >> qr >> val;
			s << qr << " " << val << endl;
		}
		return (s.str() == "'sum' 'value' 1 test1\n0 test3\n0 test2");
	}
	application &app;
};

IMPLEMENT_APP(test().app);
