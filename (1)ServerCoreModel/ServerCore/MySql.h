#pragma once

#include <stdexcept>

#include <boost/scoped_ptr.hpp>
#include <boost/thread/csbl/memory/shared_ptr.hpp>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class MySql
{
public:
	static void Test();


};



/*--------------------
	  DBConnection
--------------------*/
class DBConnection
{
	friend class DB;
public:
	DBConnection(sql::Connection* connection);
	~DBConnection();

public:
	void Reset();
	bool IsValid() { return _connection->isValid(); }

private:
	sql::Connection* _connection = nullptr;
	sql::Statement* _stmt = nullptr;
	sql::ResultSet* _res = nullptr;
	sql::PreparedStatement* _pstmt = nullptr;
	
};

/*-------------------------
	   ConnectionPool
-------------------------*/
class DBConnectionPool
{
public:
	~DBConnectionPool();

	void Init(const string& host, const string& user, const string& password,
		const string& schema, int poolSize);

public:
	DBConnection* Pop();
	void Push(DBConnection* dbConnection);

private:
	USE_LOCK;
	Queue<DBConnection*> _pool;
	sql::mysql::MySQL_Driver* _driver;
	sql::SQLString _host;
	sql::SQLString _user;
	sql::SQLString _password;
	sql::SQLString _schema;
};

/*-------------------
	   DBHelper
-------------------*/
class DB
{
public:
	DB(DBConnection* dbConnection) : _dbConnection(dbConnection) {}
	~DB();

public:
	static DB Pop();

public:
	sql::Connection* Con() { return _dbConnection->_connection; }
	sql::Statement* Stmt() { return _dbConnection->_stmt; }
	sql::ResultSet*& Res() { return _dbConnection->_res; }
	sql::PreparedStatement*& Pstmt() { return _dbConnection->_pstmt; }

private:
	DBConnection* _dbConnection;
};