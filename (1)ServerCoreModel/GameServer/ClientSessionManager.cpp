#include "pch.h"

#include "ClientSessionManager.h"
#include "ClientSession.h"

#include "ClientSession.h"
#include "Room.h"
#include "Lobby.h"

ClientSessionManager* GSessionManager = nullptr;


void ClientSessionManager::Init()
{
	GSessionManager = new ClientSessionManager();
}

void ClientSessionManager::Add(ClientSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void ClientSessionManager::Remove(ClientSessionRef client)
{
	WRITE_LOCK;
	_sessions.erase(client);

	if (RoomRef room = client->Room())
	{
		room->DoAsync(&Room::OnExitGame, client);
	}

	GLobby->DoAsync(&Lobby::RemoveClient, client);
}

void ClientSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;

	for (ClientSessionRef session : _sessions)
	{
		session->Send(sendBuffer);

		cout << "GameSessionManager.cpp : BoradCast Check" << endl;
	}
}
