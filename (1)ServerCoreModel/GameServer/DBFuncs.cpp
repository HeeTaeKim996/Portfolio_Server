#include "pch.h"
#include "DBFuncs.h"

wstring DBFuncs::GenerateID()
{
	DB db = DB::Pop();

	db.Stmt()->execute("CALL GenerateID(@out_login_id);");

	db.Res() = db.Stmt()->executeQuery("SELECT @out_login_id AS login_id");

	string st;

	while (db.Res()->next())
	{
		sql::SQLString sqlSt = db.Res()->getString("login_id");
		st = sqlSt.c_str();
	}

	return WStringCoverter::StringToWString(st);
}
