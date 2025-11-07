#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "MySql.h";

Memory* GMemory = nullptr;

ThreadManager* GThreadManager = nullptr;

DeadLockProfiler* GDeadLockProfiler = nullptr;

SendBufferManager* GSendBufferManager = nullptr;

GlobalQueue* GGlobalQueue = nullptr;

JobTimer* GJobTimer = nullptr;

DBConnectionPool* GDBConnectionPool = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GMemory = new Memory();
		GThreadManager = new ThreadManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GSendBufferManager = new SendBufferManager();
		SocketUtils::Init();
		GGlobalQueue = new GlobalQueue();
		GJobTimer = new JobTimer();
		GDBConnectionPool = new DBConnectionPool();
	}
	~CoreGlobal()
	{
		delete GMemory;
		delete GThreadManager;
		delete GDeadLockProfiler;
		delete GSendBufferManager;
		SocketUtils::Clear();
		delete GGlobalQueue;
		delete GJobTimer;
		delete GDBConnectionPool;
	}
}GCoreGlobal;