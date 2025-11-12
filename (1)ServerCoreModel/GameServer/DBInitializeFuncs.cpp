#include "pch.h"
#include "DBInitializeFuncs.h"
#include "MySql.h"

void DBInitializeFuncs::InitializeSettings()
{

}

void DBInitializeFuncs::InitializeUserInfoTable()
{
	cout << "Sure You Want To Delete DB? " << endl;
	cout << "-> ";
	string ans;
	cin >> ans;
	if (ans == "yes")
	{
		try
		{
			const char* sql = R"(
				CREATE TABLE user_infos(
					id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
					login_id CHAR(10) NOT NULL UNIQUE,
					best_score INT UNSIGNED DEFAULT 0,
					nick_name VARCHAR(12) NOT NULL,
					INDEX idx_best_score(best_score)
				) AUTO_INCREMENT = 1
				  DEFAULT CHARSET = utf8mb4;
			)";
			{
				DB db = DB::Pop();

				db.Stmt()->executeUpdate
				("DROP TABLE IF EXISTS user_infos");

				sql::SQLString sqlSt(sql);

				db.Stmt()->executeUpdate(sqlSt);
			}
		}
		catch (sql::SQLException& e)
		{
			cout << e.what() << endl;
			cout << e.getErrorCode() << endl;
			cout << e.getSQLState() << endl;
		}





	}
	else
	{
		assert(0);
	}




}


void DBInitializeFuncs::EnableCreatingFunction()
{
	DB db = DB::Pop();
	db.Stmt()->executeUpdate("SET GLOBAL log_bin_trust_function_creators = 1;");
}

void DBInitializeFuncs::Create_Func_GenerateID()
{
	{
		DB db = DB::Pop();
		db.Stmt()->executeUpdate("DROP PROCEDURE IF EXISTS GenerateID;");
	}

	const char* sql = R"(
		CREATE PROCEDURE GenerateID(OUT generate_login_id CHAR(10))
		BEGIN
			DECLARE result CHAR(10) DEFAULT '';
			DECLARE i INT;
			DECLARE	ch CHAR(1);
			DECLARE generate_nick_name VARCHAR(12) DEFAULT '';
			DECLARE nick_name_rand FLOAT;


			WHILE result = '' OR EXISTS( SELECT 1 FROM user_infos WHERE login_id = result) DO
				SET result = '';
				SET i = 0;
				WHILE i < 10 DO
					IF RAND() < 0.5 THEN
						SET ch = CAST(CHAR(FLOOR(RAND() * 26) + 65) AS CHAR);
					ELSE
						SET ch = CAST(CHAR(FLOOR(RAND() * 26) + 97) AS CHAR);
					END IF;

					SET result = CONCAT(result, ch);
					SET i = i + 1;
				END WHILE;
			END WHILE;
			
			SET nick_name_rand = RAND();
			IF nick_name_rand < 0.25 THEN
				SET generate_nick_name = 'SmilingApeach';
			ELSEIF nick_name_rand < 0.5 THEN
				SET generate_nick_name = 'JumpingHarry';
			ELSEIF nick_name_rand < 0.75 THEN
				SET generate_nick_name = 'YawningBorry';
			ELSE
				SET generate_nick_name = 'WoaaLion'; 
			END IF;
			

			INSERT INTO user_infos VALUES(NULL, result, NULL , generate_nick_name);

			SET generate_login_id = result;
		END;
)";
	
	{
		DB db = DB::Pop();
		sql::SQLString sqlSt(sql);
		db.Stmt()->executeUpdate(sqlSt);
	}
}
