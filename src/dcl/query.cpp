/*
 * query.cpp
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

#include <dcl/query.h>
#include <dcl/strutils.h>

namespace dbp {
namespace odbc {

using namespace std;

std::string query::field::get_value() const {
	string rslt;
	_query->get_field(number, rslt);
	return rslt;
}

query::query(const connection &connection): _db(connection),
  _prmcnt(0), _fldcnt(0), stmt(0) {
	// allocate statement handle
	SQLRETURN r = SQLAllocHandle(SQL_HANDLE_STMT, _db.hdbc, &stmt);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw query_exception(
		  _("Can't allocate ODBC statement handle"));
	}
}

query::~query() {
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

std::string query::parse(const std::string &statement) {
	// clear existing parameters
	_parameters.clear();
	// parsing the query to obtain its parameters
	string q;
	bool ssq = false, dsq = false;
	string::const_iterator i = statement.begin();
	while (i != statement.end()) {
		switch (*i) {
			case '\'':
				ssq = not ssq;
				q += *i;
				break;
			case '"':
				dsq = not dsq;
				q += *i;
				break;
			case '?':
				if (!ssq && !dsq) {
					_parameters.push_back(query::parameter());
				}
				q += *i;
				break;
			case ':':
				if (!ssq && !dsq) {
					query::parameter p;
					++i;
					while ((i != statement.end()) && (isalnum(*i) || (*i == '_'))) {
						p.name += *i;
						++i;
					}
					--i;
					_parameters.push_back(p);
					q += '?';
				} else {
					q += *i;
				}
				break;
			default:
				q += *i;
				break;
		}
		++i;
	}
	return q;
}

query::parameters& query::prepare(const std::string &statement) {
	// prepare SQL statement
	string q = parse(statement);
	SQLRETURN r = SQLPrepare(stmt, (SQLCHAR*)q.c_str(), SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw query_exception(
		  (format(_("Can't prepare SQL statement: {0}")) %
		  get_error()).str());
	}
	return _parameters;
}

query& query::operator()(const std::string &statement) {
	prepare(statement);
	return *this;
}

void query::retrieve_metadata() {
	SQLSMALLINT cnt;
	SQLRETURN r = SQLNumResultCols(stmt, &cnt);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw query_exception(
		  (format(_("Can't obtain a columns count: {0}")) %
		  get_error()).str());
	}
	for (int i = 1; i <= cnt; i++) {
		SQLSMALLINT len_real;
		SQLSMALLINT data_type;
		SQLULEN col_length;
		SQLSMALLINT digits;
		SQLSMALLINT nullable;
		SQLCHAR col_name[256];
		r = SQLDescribeCol(stmt, i, col_name, sizeof(col_name),
		  &len_real, &data_type, &col_length, &digits, &nullable);
		if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
			throw query_exception(
			  (format(_("Can't obtain column metadata: {0}")) %
			  get_error()).str());
		}
		field f(this);
		f.name = (char*)col_name;
		f.number = i - 1;
		_fields.push_back(f);
	}
}

const query::fields& query::execute() {
	// close query cursor if any
	SQLCloseCursor(stmt);
	// bind query parameters
	int num = 1;
	SQLRETURN r;
	SQLLEN _null = SQL_NULL_DATA;
	SQLLEN _nts = SQL_NTS;
	for (parameters::const_iterator i = _parameters.begin();
	  i != _parameters.end(); ++i) {
	  	if (i->value.empty())
			r = SQLBindParameter(stmt, num,
			  SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0,
			  0, NULL, 0, &_null);
		else
			r = SQLBindParameter(stmt, num,
	  		  SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, i->value.length(),
			  0, (SQLCHAR*)i->value.c_str(), 0, &_nts);
		if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
			throw query_exception(
			  (format(_("Can't assign SQL parameter: {0}")) %
			  get_error()).str());
		}
		num++;
	}
	// execute prepared query
	r = SQLExecute(stmt);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO) && (r != SQL_NO_DATA)) {
		throw query_exception(
		  (format(_("Can't execute SQL statement: {0}")) %
		  get_error()).str());
	}
	// obtain recordset information if any
	retrieve_metadata();
	return _fields;
}

const query::fields& query::execute(const std::string &statement) {
	// close query cursor if any
	SQLCloseCursor(stmt);
	// execute SQL statement
	SQLRETURN r = SQLExecDirect(stmt, (SQLCHAR*)statement.c_str(), SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO) && (r != SQL_NO_DATA)) {
		throw query_exception(
		  (format(_("Can't execute SQL statement: {0}")) %
		  get_error()).str());
	}
	// obtain recordset information if any
	retrieve_metadata();
	return _fields;
}

void query::set_parameter(int num, int value) {
	set_parameter(num, to_string<int>(value));
}

void query::set_parameter(int num, double value) {
	set_parameter(num, to_string<double>(value));
}

void query::set_parameter(int num, const std::string &value) {
	_parameters.at(num).value = value;
}

void query::set_parameter(int, const flush&) {
	execute();
	// set to -1 because << operator inrements this after executing,
	// so counter reseted to 0
	_prmcnt = -1;
}

bool query::next() {
	_fldcnt = 0;
	SQLRETURN r = SQLFetch(stmt);
	if (r == SQL_SUCCESS) {
		return true;
	}
	if (r != SQL_NO_DATA) {
		// check for invalid cursor state (no result set)
		if (r == SQL_ERROR) {
			char odbc_error[6] = {0};
			SQLINTEGER native_error = 0;
			SQLSMALLINT message_len = 0;
			SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, (SQLCHAR*)&odbc_error,
			  &native_error, NULL, 0, &message_len);
			if (string(odbc_error) == string("24000")) return false;
		}
		throw query_exception(
		  (format(_("Can't fetch the data row: {0}")) %
		  get_error()).str());
	}
	return false;
}

void query::get_field(int num, int &value) {
	string s;
	get_field(num, s);
	value = from_string<int>(s);
}

void query::get_field(int num, double &value) {
	string s;
	get_field(num, s);
	value = from_string<double>(s);
}

void query::get_field(int num, std::string &value) {
	stringstream s;
	static const SQLLEN buf_size = 255;
	SQLCHAR buffer[buf_size + 1];
	SQLLEN size;
	SQLRETURN r;
	while ((r = SQLGetData(stmt, num + 1, SQL_C_CHAR, &buffer, sizeof(buffer),
	  &size)) != SQL_NO_DATA) {
		if (r == SQL_ERROR) {
			throw query_exception(
			  (format(_("Can't retrieve data for column: {0}")) %
			  get_error()).str());
		}
		if (r == SQL_SUCCESS) {
			s.write((char*)&buffer, size);
			break;
		}
		int num_bytes = (size > buf_size) ||
		  (size == SQL_NO_TOTAL) ? buf_size : size;
		s.write((char*)&buffer, num_bytes);
	}
	value = s.str();
}

std::string query::get_error() const {
	char odbc_error[6];
	SQLINTEGER native_error;
	char message[SQL_MAX_MESSAGE_LENGTH];
	SQLSMALLINT message_len;
	string rslt;
	if (SQLGetDiagRec(SQL_HANDLE_STMT, stmt, 1, (SQLCHAR*)&odbc_error,
	  &native_error, (SQLCHAR*)&message,
	  sizeof(message), &message_len) == SQL_SUCCESS) {
		rslt = "[" + string(odbc_error) + "]" + string(message);
	}
	return rslt;
}

}} // namespace

