/*
 * connection.cpp
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

#include <dcl/connection.h>
#include <dcl/singleton.h>
#include <dcl/strutils.h>

namespace dbp {
namespace odbc {

using namespace std;

class environment: public singleton<environment> {
	friend class singleton<environment>;
public:
	virtual ~environment() {
		if (henv)
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
	};
	SQLHENV& handle() {
		return henv;
	}
private:
	SQLHENV henv;
	environment(): henv(NULL) {
		// will use pooling
		SQLRETURN r = SQLSetEnvAttr(
			NULL,  // make process level cursor pooling
			SQL_ATTR_CONNECTION_POOLING,
			(SQLPOINTER)SQL_CP_ONE_PER_DRIVER,
			SQL_IS_INTEGER
		);
		if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
			throw connection_exception(
			  _("Can't initialize ODBC environment"));
		}
		// setup ODBC environment
		r = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
		if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
			henv = NULL;
			throw connection_exception(
			  _("Can't allocate ODBC environment handle"));
		}
		// setup ODBC version required
		r = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,
		  (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
		if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
			throw connection_exception(
			  _("Can't setup ODBC environment version"));
		}
	};
};

connection::connection(): hdbc(NULL), _is_open(false) {
	// setup ODBC connection
	SQLHENV &henv = environment::instance().handle();
	SQLRETURN r = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		hdbc = NULL;
		throw connection_exception(
		  _("Can't allocate ODBC connection handle"));
	}
}

connection::~connection() {
	close();
	if (hdbc)
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
}

std::string connection::get_error() const {
	char odbc_error[6];
	SQLINTEGER native_error;
	char message[SQL_MAX_MESSAGE_LENGTH];
	SQLSMALLINT message_len;
	string rslt;
	if (SQLGetDiagRec(SQL_HANDLE_DBC, hdbc, 1, (SQLCHAR*)&odbc_error,
	  &native_error, (SQLCHAR*)&message,
	  sizeof(message), &message_len) == SQL_SUCCESS) {
		rslt = "[" + string(odbc_error) + "]" + string(message);
	}
	return rslt;
}

void connection::open(const std::string &dsn, const std::string &user_name,
  const std::string &password) {
	close();
	SQLRETURN r = SQLConnect(hdbc, (SQLCHAR*)dsn.c_str(), SQL_NTS,
	  (SQLCHAR*)user_name.c_str(), SQL_NTS,
	  (SQLCHAR*)password.c_str(), SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Can't open the database {0}: {1}")) % dsn %
		  get_error()).str());
	}
	_is_open = true;
}

void connection::open(const std::string &connect_string) {
	close();
	SQLCHAR dummy[SQL_MAX_OPTION_STRING_LENGTH];
	SQLSMALLINT dummy_size;
	SQLRETURN r = SQLDriverConnect(hdbc, NULL,
	  (SQLCHAR*)connect_string.c_str(), connect_string.length(),
	  dummy, sizeof(dummy), &dummy_size, SQL_DRIVER_COMPLETE);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Can't open the database: {0}")) % get_error()).str());
	}
	_is_open = true;
}

void connection::close() {
	if (_is_open)
		SQLDisconnect(hdbc);
	_is_open = false;
}

bool connection::is_open() const {
	return _is_open;
}

void connection::begin_transaction(transaction_isolation level) {
	SQLINTEGER l;
	switch (level) {
		case READ_UNCOMMITTED:
			l = SQL_TXN_READ_UNCOMMITTED;
			break;
		case READ_COMMITTED:
			l = SQL_TXN_READ_COMMITTED;
			break;
		case REPEATABLE_READ:
			l = SQL_TXN_REPEATABLE_READ;
			break;
		case SERIALIZABLE:
			l = SQL_TXN_SERIALIZABLE;
			break;
		default:
			throw connection_exception(
			  _("Invalid transaction isolation level"));
	}
	SQLRETURN r = SQLSetConnectAttr(hdbc, SQL_ATTR_TXN_ISOLATION,
	  (SQLPOINTER)l, SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Can't setup the transaction isolation level: {0}")) %
		  get_error()).str());
	}
	r = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT,
	  (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Can't turn off autocommit mode: {0}")) %
		  get_error()).str());
	}
}

void connection::commit() {
	SQLRETURN r = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_COMMIT);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Transaction commit failed: {0}")) %
		  get_error()).str());
	}
	r = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT,
	  (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Can't turn on autocommit mode: {0}")) %
		  get_error()).str());
	}
}

void connection::rollback() {
	SQLRETURN r = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_ROLLBACK);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Transaction rollback failed: {0}")) %
		  get_error()).str());
	}
	r = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT,
	  (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
	if ((r != SQL_SUCCESS) && (r != SQL_SUCCESS_WITH_INFO)) {
		throw connection_exception(
		  (format(_("Can't turn on autocommit mode: {0}")) %
		  get_error()).str());
	}
}

void connection::init_environment() {
	environment::instance();
}

}} // namespace

