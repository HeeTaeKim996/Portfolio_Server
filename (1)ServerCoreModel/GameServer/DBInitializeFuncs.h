#pragma once
class DBInitializeFuncs
{
public:
	inline static void InitializeFuncs()
	{
		EnableCreatingFunction();

		Create_Func_GenerateID();
	}


public:
	static void InitializeSettings();

public:
	static void InitializeUserInfoTable();


private:
	static void EnableCreatingFunction();


	static void Create_Func_GenerateID();
};

