#include"pch.h"

#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "FlatBufferWriter.h"
#include "ClientPacketHandler.h"
#include "Job.h"
#include "Room.h"
#include "ObjectPool.h"
#include "Lobby.h"
#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 
#include "DBInitializeFuncs.h"

void DBSample()
{
	{
		DB db = DB::Pop();

		db.Res() = db.Stmt()->executeQuery("SELECT * FROM sampletable");

		while (db.Res()->next())
		{
			cout << "ID : " << db.Res()->getInt("int_sample") << endl;

			cout << db.Res()->getString("wchar_sample").c_str() << endl;
		}
	}

	{
		DB db = DB::Pop();

		db.Pstmt() = db.Con()->prepareStatement
		("SELECT * FROM sampletable WHERE int_sample = 2;");

		db.Res() = db.Pstmt()->executeQuery();

		while (db.Res()->next())
		{
			cout << "ID : " << db.Res()->getInt("int_sample") << endl;

			cout << db.Res()->getString("wchar_sample").c_str() << endl;
		}
	}
}

enum : uint64
{
	GLOBAL_QUEUE_MONITORING_TICK = 64,
};


void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + GLOBAL_QUEUE_MONITORING_TICK;

		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();

		ThreadManager::DoGlobalQueueWork();
	}
}


static uint64 mainThreadTickCount = 0;
void DoMainThreadWorkJob(ServerServiceRef& service)
{
	while (true)
	{
		if (GetTickCount64() > mainThreadTickCount + 1'000)
		{
			{
#ifdef _MEMORY_COUNT
				cout 
					<< "ClientSession(" << MemoryCount::ClientSessionCount() << ") / "
					<< "Player(" << MemoryCount::PlayerCount() << ") / "
					<< "CapsuleRoom(" << MemoryCount::CapsuleRoomCount() << ") / "
					<< "MultiRoom(" << MemoryCount::MultiRoomCount() << ") / "
					<< "BasePivot(" << MemoryCount::BasePivotCount() << ")"
					<< "SlickPivot(" << MemoryCount::SlickPivotCount() << ")"
					<< "BlueEnergy(" << MemoryCount::BlueEnergyCount() << ")" 
					<< "LoosePivot(" << MemoryCount::LoosePivotCount() << ")"
					<< "StrongBird(" << MemoryCount::StrongBirdCount() << ")"
					<< endl;
#endif 

			}

			mainThreadTickCount = GetTickCount64();
		}

		{ // DoWorkJob
			LEndTickCount = ::GetTickCount64() + GLOBAL_QUEUE_MONITORING_TICK;

			// 네트워크 입출력 처리 -> 인게임 로직(패킷 핸들러에 의해)
			service->GetIocpCore()->Dispatch(10);
			// time out을 10ms로 두어, GetQeueCompletionStatus가 10ms경과시, false로 처리되어, 빠져나오게함


			// 예약된 일감 처리
			ThreadManager::DistributeReservedJobs();


			// 글로벌 큐 
			ThreadManager::DoGlobalQueueWork();
		}
	}
}



void Init(string password)
{
	if (E_MULTI_HEIGHT_POOL_MAX_COUNT >= 255 || E_CAPSULE_HEIGHT_POOL_MAX_COUNT >= 255)
	{
		CRASH("255 MEANS NULL");
	}


	ClientSessionManager::Init();
	Lobby::Init();
	
	ClientPacketHandler::Init();
	GDBConnectionPool->Init("tcp://127.0.0.1:3306", "root", password.c_str(), "flying_monkeys", 1);
	
}


int main()
{
	string password;

	printf("DB Password : ");
	cin >> password;
	Init(password);
	printf("\n");
	
	DBInitializeFuncs::InitializeSettings();
	DBInitializeFuncs::InitializeFuncs();



	ServerServiceRef service = MakeShared<ServerService>(NetAddress(L"172.30.1.97", 7777),
		MakeShared<IocpCore>(), []()
		{
			return ObjectPool<ClientSession>::MakeShared();
		}, 100);


	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}



	DoMainThreadWorkJob(service); // Main Thread Also Work


	GThreadManager->Join();
}