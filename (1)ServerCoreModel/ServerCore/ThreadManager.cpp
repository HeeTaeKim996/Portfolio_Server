#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"


/*---------------------------
		 ThreadManager
---------------------------*/


ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	WRITE_LOCK;

	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	WRITE_LOCK;

	for (thread& thr : _threads)
	{
		if (thr.joinable())
		{
			thr.join();
		}
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > LEndTickCount)
		{
			break;
		}

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
		{
			break;
		}
		jobQueue->Execute();
	}
}

void ThreadManager::DistributeReservedJobs()
{
	const uint64 now = GetTickCount64();

	GJobTimer->Distribute(now);
}
