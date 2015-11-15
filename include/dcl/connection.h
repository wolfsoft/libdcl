/*
 * connection.h
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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <string>

#ifdef WIN32
#include <windows.h>
#include <odbcinst.h>
#endif

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <dcl/exception.h>

namespace dbp {
namespace odbc {

//! ODBC connection exception
/*!
	This exception class is used to signal about ODBC connection errors.
*/
class connection_exception: public dbp::exception {
public:
	//! Constructor
	connection_exception(const std::string &msg): exception(msg) { }
};

//!	ODBC database connection
/*!
	This class represents the database connection via ODBC.
*/
class connection {
	friend class query;
public:
	//! Transaction isolation level
	typedef enum {
		READ_UNCOMMITTED,
		READ_COMMITTED,
		REPEATABLE_READ,
		SERIALIZABLE
	} transaction_isolation;
	//! Constructor
	connection();
	//! Destructor
	virtual ~connection();
	//! Connect to database
	/*!
		Opens the connection to the database.
		
		\param dsn the ODBC connection name.
		\param user_name the user login.
		\param password the user password.
	*/
	void open(const std::string &dsn, const std::string &user_name,
	  const std::string &password);
	//! Connect to database
	/*!
		Opens the connection to the database.
		
		\param connect_string the full ODBC connection string.
	*/
	void open(const std::string &connect_string);
	//! Check the connection state
	/*!
		Returns true if the connection is opened and false otherwise.

		\returns the connection state.
	*/
	bool is_open() const;
	//! Disconnect from database
	/*!
		Closes the connection to the database.
	*/
	void close();
	//! Start the transaction
	/*!
		Turns off default autocommit mode and starts the transaction. Use
		commit or rollback methods to save or cancel changes.
		
		\param level the transaction isolation level
	*/
	void begin_transaction(transaction_isolation level = READ_COMMITTED);
	//! Commit the transaction
	/*!
		Save changes since last begin_transaction call. After commiting the
		transaction the autocommit mode is turning on.
	*/
	void commit();
	//! Roll back the transaction
	/*!
		Cancel changes since last begin_transaction call. After rolling back the
		transaction the autocommit mode is turning on.
	*/
	void rollback();
	//! Initializes the odbc environment
	/*!
		Call this function in the main thread of your multithreading application.
	*/
	static void init_environment();
protected:
	std::string get_error() const;
private:
	SQLHDBC hdbc;
	bool _is_open;
};

}} // namespace

#endif /*_CONNECTION_H_*/

