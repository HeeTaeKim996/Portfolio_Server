/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/
#pragma once
#include <thread>
#include <functional>

/*---------------------------
	     ThreadManager
---------------------------*/
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(function<void(void)> callback);
	void Join();

private:
	void InitTLS();
	void DestroyTLS();

public:
	static void DoGlobalQueueWork();
	static void DistributeReservedJobs();

private:
	USE_LOCK;
	Vector<thread> _threads;
};