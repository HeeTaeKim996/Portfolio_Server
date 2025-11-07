#include "pch.h"
#include "Lobby.h"

#include "ClientSession.h"
#include "Room.h"
#include "CapsuleRoom.h"
#include "MultiRoom.h"

#include "FlatBufferWriter.h";
#include "ClientSessionManager.h"

shared_ptr<Lobby> GLobby = nullptr;

void Lobby::Init()
{
	GLobby = MakeShared<Lobby>();
}

Lobby::Lobby()
{

}

void Lobby::AcceptClient(ClientSessionRef client)
{
	_clients.insert(client);

	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_ENTER_LOBBY);
	}
	client->Send(sendBuffer);

	InformLobbyInfos();
}

void Lobby::RemoveClient(ClientSessionRef client)
{
	_clients.erase(client); 

	InformLobbyInfos();
}



void Lobby::MakeCapsuleRoom(ClientSessionRef client)
{
	uint64 mastersId = client->PlayerId();
	if (_capsuleRooms.find(mastersId) == _capsuleRooms.end())
	{
		_capsuleRooms[mastersId] = ObjectPool<CapsuleRoom>::MakeShared(mastersId);
		_capsuleRooms[mastersId]->Enter(client);
		_capsuleRooms[mastersId]->DoTimer(CapsuleRoom::MATCH_START_TICK, &CapsuleRoom::MatchingInitialize);

		_clients.erase(client); // EXIT_LOBBY


		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_ENTER_ROOM);
		}
		client->Send(sendBuffer);
	}


	InformLobbyInfos();
}


void Lobby::RemoveCapsuleRoom(uint64 roomId)
{
	_matchingRooms.erase(weak_ptr<CapsuleRoom>(_capsuleRooms[roomId]));

	_capsuleRooms.erase(roomId);


	InformLobbyInfos();
}

void Lobby::OnClientDisconnected(ClientSessionRef client)
{

}



uint64 Lobby::GetAllRoomsPlayersCount()
{
	uint64 sum = 0;

	for (pair<uint64, CapsuleRoomRef> room : _capsuleRooms)
	{
		sum += room.second->PlayersCount();
	}

	return sum;
}

void Lobby::InformLobbyInfos()
{
	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_INFORM_LOBBY_INFOS);

		fbw.Write(static_cast<uint32>(_capsuleRooms.size()));

		for (pair<uint64, CapsuleRoomRef> room : _capsuleRooms)
		{
			fbw.Write(static_cast<uint64>(room.first));
		}
	}

	for (ClientSessionRef client : _clients)
	{
		client->Send(sendBuffer);
	}
}


/*---------------------
	  RoomMatching
---------------------*/
void Lobby::Match(CapsuleRoomRef matchingRoom, UINT32 score)
{
	for (auto it = _matchingRooms.begin(); it != _matchingRooms.end(); it++)
	{
		if (std::abs((INT32)(score - it->second)) < 20)
		{
			if (CapsuleRoomRef room = it->first.lock())
			{
				if (matchingRoom != room && room->GetMatchState() == CapsuleRoom::MatchState::Matching)
				{
					MultiRoomRef multiRoom = ObjectPool<MultiRoom>::MakeShared(matchingRoom, room);
					_multiRooms.insert(multiRoom);

					_matchingRooms.erase(it);

					matchingRoom->OnMatched(multiRoom);
					room->OnMatched(multiRoom);

					return;
				}
			}
		}
	}

	weak_ptr<CapsuleRoom> weak = matchingRoom;
	_matchingRooms[weak] = score;
}