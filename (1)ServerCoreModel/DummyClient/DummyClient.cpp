#include "pch.h"

#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

#include "ServerPacketHandler.h"
#include "FlatBufferWriter.h"
#include "ServerSession.h"

#include <iostream>
#include <string>

void DoBlabber(ClientServiceRef service)
{
	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();

	{
		FlatBufferWriter fbw(sendBuffer, C_TEST_CHAT);
		wstring wstr = L"Blabber";
		fbw.WriteWString(wstr);
	}

	while (true)
	{
		service->Broadcast(sendBuffer);

		this_thread::sleep_for(20ms);
	}
}


int main()
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(NetAddress(L"127.0.0.1", 7777), MakeShared<IocpCore>(), 
		MakeShared<ServerSession>, 1);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	thread blabberThrad;


Lobby:
	while (true)
	{
		
		cout << "MakeRoom : 0 // JoinRoom : 1";

		uint16 intKey;
		cin >> intKey;

		if (intKey == 0)
		{
			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, C_MAKE_ROOM);
			}

			service->Broadcast(sendBuffer);

			break;
		}
		else if (intKey == 1)
		{
			cout << "Entering Room Id : ";
			uint64 roomId;
			cin >> roomId;



			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, C_JOIN_ROOM);
				fbw.Write(roomId);
			}

			service->Broadcast(sendBuffer);

			break;
		}		
	}

Room:
	while (true)
	{
		wstring input;
		std::getline(std::wcin, input);

		//wstring wstr = L"Test";

		if (input == L"Blabber")
		{
			if (blabberThrad.joinable() == false)
			{
				blabberThrad = thread(DoBlabber, service);
			}
		}
		else if (input == L"Exit_Room")
		{
			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, C_BACK_TO_LOBBY);
			}
			service->Broadcast(sendBuffer);

			if (blabberThrad.joinable())
			{
				blabberThrad.join();
			}

			goto Lobby;
		}
		else
		{
			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, C_TEST_CHAT);
				fbw.WriteWString(input);
			}
			service->Broadcast(sendBuffer);
		}
	}

	


	GThreadManager->Join();
}

