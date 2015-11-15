/*
 * query.h
 * This file is part of dbPager Classes Library (DCL)
 *
 * Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
 *
 * DCL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 *
 * DCL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DCL; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#ifndef _QUERY_H_
#define _QUERY_H_

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <odbcinst.h>
#endif

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <dcl/connection.h>
#include <dcl/exception.h>
#include <dcl/strutils.h>

namespace dbp {
namespace odbc {

//!	The ODBC query exception
/*!
	The exception class is used to signal about errors in query class.
*/
class query_exception: public dbp::exception {
public:
	//! Constructor
	query_exception(const std::string &msg): exception(msg) { }
};

//!	The ODBC query
/*!
	The query class represents the single SQL statement. By using this class
	you can execute any SQL statement to insert, update, delete records
	from database, select data from database and execute stored procedures.
*/
class query {
public:
	//! Query parameter
	/*!
		The parameter class represents the single SQL statement parameter.
	*/
	class parameter {
	public:
		std::string name;
		std::string value;
	};
	//! The collection of query parameters
	typedef std::vector<parameter> parameters;
	//! Recordset field
	/*!
		The field class represents the column of the SQL query execution result.
	*/
	class field {
	public:
		field(query *parent = NULL): _query(parent) { };
		int number;
		std::string name;
		std::string get_value() const;
	private:
		query *_query;
	};
	//! The recordset
	typedef std::vector<field> fields;
	//! Special class to execute the query via << operator.
	class flush { };
	//! Constructor
	/*!
		Initializes the query by odbc connection.

		\param connection the reference to the odbc connection.
	*/
	query(const connection &connection);
	//! Destructor
	/*!
		Destroys the query object and releases all its allocated resources.
	*/
	virtual ~query();
	//! Prepare the SQL statement
	/*!
		Prepares the SQL statement to execute.

		\param statement the SQL statement.
		\return the parsed query parameters.
	*/
	parameters& prepare(const std::string &statement);
	//! SQL statement assignment
	/*!
		The syntax sugar for the query::prepare method.

		\param statement the SQL statement.
	*/
	query& operator()(const std::string &statement);
	//! Execute the prepared SQL statement
	/*!
		Executes prevuosly prepared SQL statement.

		\return the record set fields.
	*/
	const fields& execute();
	//! Direct execute the SQL statement
	/*!
		Executes SQL statement without preparing.

		\param statement the SQL statement.
		\return the record set fields.
	*/
	const fields& execute(const std::string &statement);
	//! Fetch resultset row
	/*!
		Retrieves the next row from resultset and fills the field values with
		the data retrieved.

		\return true, if row is available and false when not.
	*/
	bool next();
	//! Set query parameter value
	void set_parameter(int num, int value);
	//! Set query parameter value
	void set_parameter(int num, double value);
	//! Set query parameter value
	void set_parameter(int num, const std::string &value);
	//! Query parameter assignment operator
	/*!
		Assigns a parameter value to the prepared query. Consecutive execution
		of the operator assigns the next parameter. To execute the query,
		pass the special flush object: the query will be executed and you can
		assign another set of parameters, for example:
		\code
query q(db);
q("insert into test1 (id, value) values (?, ?)")
  << 1 << "test1" << query::flush()
  << 2 << "test2" << query::flush()
  << 3 << "test3" << query::flush();
		\endcode
	*/
	template<class T>
	query& operator<<(const T &parameter) {
		set_parameter(_prmcnt, parameter);
		_prmcnt++;
		return *this;
	};
	//! Get field value
	void get_field(int num, int &value);
	//! Get field value
	void get_field(int num, double &value);
	//! Get field value
	void get_field(int num, std::string &value);
	//! Query result assignment operator
	template<class T>
	query& operator>>(T &var) {
		get_field(_fldcnt, var);
		_fldcnt++;
		return *this;
	};
private:
	const connection &_db;
	int _prmcnt, _fldcnt;
	SQLHSTMT stmt;
	parameters _parameters;
	fields _fields;
	// Obtain the last diagnostic (error) message from ODBC system
	std::string get_error() const;
	// Retrieve resultset metadata after query executing
	void retrieve_metadata();
	void set_parameter(int num, const flush &value);
	// Parse the query, filling the parameter list and reconstruct the statement
	std::string parse(const std::string &statement);
};

}} // namespace

#endif /*_QUERY_H_*/
