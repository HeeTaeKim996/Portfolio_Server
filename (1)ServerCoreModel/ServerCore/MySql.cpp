#include "pch.h"
#include "MySql.h"

void MySql::Test()
{
	try
	{
		sql::mysql::MySQL_Driver* driver;
		sql::Connection* con;
		sql::Statement* stmt;
		sql::ResultSet* res;
		sql::PreparedStatement* pstmt;


		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "kimht085213!");
		con->setSchema("flying_monkeys");

		stmt = con->createStatement();
		
		/*
		pstmt = con->prepareStatement("INSERT INTO sampletable (int_sample, wchar_sample) VALUES (?, ?)");
		pstmt->setInt(1, 3);
		pstmt->setString(2, "Bye");
		pstmt->executeUpdate();
		*/

		res = stmt->executeQuery("SELECT * FROM sampletable");

		while (res->next())
		{
			std::cout << "ID : " << res->getInt("int_sample") << endl;
			
			sql::SQLString sqlString = res->getString("wchar_sample");
			std::string st(sqlString.c_str(), sqlString.length());
			wstring wst = WStringCoverter::StringToWString(st);

			wcout << wst << endl;
		}

		delete res;
		delete stmt;
		delete con;

	}
	catch (sql::SQLException& e)
	{
		std::cerr << "SQL Exception : " << e.what() << std::endl;
		std::cerr << "MySQL Err Code : " << e.getErrorCode() << std::endl;
	}

	
}



/*--------------------
	  DBConnection
--------------------*/
DBConnection::DBConnection(sql::Connection* connection) : _connection(connection)
{
	_stmt = connection->createStatement();
}

DBConnection::~DBConnection()
{
	Reset();
	delete _connection;
}

void DBConnection::Reset()
{
	if (_res)
	{
		delete _res;
		_res = nullptr;
	}
	if (_pstmt)
	{
		delete _pstmt;
		_pstmt = nullptr;
	}
}


/*-------------------------
	   ConnectionPool
-------------------------*/
DBConnectionPool::~DBConnectionPool()
{
	WRITE_LOCK;
	while (!_pool.empty())
	{
		ObjectPool<DBConnection>::Push(_pool.front());
		_pool.pop();
	}
}

void DBConnectionPool::Init(const string& host, const string& user, const string& password,
	const string& schema, int poolSize)
{
	_host = sql::SQLString(host.c_str());
	_user = sql::SQLString(user.c_str());
	_password = sql::SQLString(password.c_str());
	_schema = sql::SQLString(schema.c_str());

	_driver = sql::mysql::get_mysql_driver_instance();
	
	
	WRITE_LOCK;
	for (int i = 0; i < poolSize; i++)
	{
		sql::Connection* con = _driver->connect(_host, _user, _password);

		con->setSchema(_schema);
		DBConnection* dbConnection = ObjectPool<DBConnection>::Pop(con);
		_pool.push(dbConnection);
	}
	
}
DBConnection* DBConnectionPool::Pop()
{
	DBConnection* retCon = nullptr;

	{
		WRITE_LOCK;
		if (_pool.empty())
		{
			sql::Connection* con = _driver->connect(_host, _user, _password);
			con->setSchema(_schema);
			retCon = ObjectPool<DBConnection>::Pop(con);


			return retCon;
		}

		retCon = _pool.front();
		_pool.pop();
	}


	if (!retCon->IsValid())
	{
		ObjectPool<DBConnection>::Push(retCon);

		sql::Connection* con = _driver->connect(_host, _user, _password);
		con->setSchema(_schema);
		retCon = ObjectPool<DBConnection>::Pop(con);
	}

	return retCon;
}

void DBConnectionPool::Push(DBConnection* dbConnection)
{
	dbConnection->Reset();
	WRITE_LOCK;
	_pool.push(dbConnection);
}


/*-------------------
	   DBHelper
-------------------*/

DB::~DB()
{
	GDBConnectionPool->Push(_dbConnection);
}

DB DB::Pop()
{	
	return GDBConnectionPool->Pop();
}
