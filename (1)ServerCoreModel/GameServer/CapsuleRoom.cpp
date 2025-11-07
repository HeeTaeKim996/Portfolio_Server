#include "pch.h"
#include "CapsuleRoom.h"

#include "FlatBufferWriter.h"
#include "FlatBufferReader.h"

#include "ClientSession.h"
#include "Lobby.h"
#include "Player.h"
#include "Pivot.h"
#include "BasePivot.h"
#include "LoosePivot.h"
#include "MultiRoom.h"
#include "Lobby.h"


#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 



/*---------------------
	  RoomMatching
---------------------*/

CapsuleRoom::CapsuleRoom(UINT64 roomId)
	: Room(roomId, E_CAPSULE_HEIGHT_POOL_MAX_COUNT), _matchState(MatchState::Sleep)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddCapsuleRoom();
#endif 

}

CapsuleRoom::~CapsuleRoom()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveCapsuleRoom();
#endif 

}

void CapsuleRoom::MatchingInitialize()
{
	DoTimer(MATCH_START_TICK, &CapsuleRoom::StartMatch);
}

void CapsuleRoom::StartMatch()
{
	_matchState = MatchState::Matching;
	MatchCall();
}

void CapsuleRoom::MatchCall()
{
	if (_matchState != MatchState::Matching)
	{
		return;
	}

	GLobby->DoAsync(&Lobby::Match, static_pointer_cast<CapsuleRoom>(shared_from_this()), _roomScore);

	DoTimer(MATCH_CHECK_INTERVAL, &CapsuleRoom::MatchCall);
}

void CapsuleRoom::OnRoomTransferFinished()
{
	_multiRoom.reset();

	for (int i = 0; i < _heightObjects.size(); i++)
	{
		_heightObjects[i] = nullptr;
	}
}

void CapsuleRoom::RetrievePlayer(RetrievingRoomStruct retrievingRoomStruct, MultiRoomRef multiRoom)
{
	std::copy(retrievingRoomStruct.heightObjects.begin(), retrievingRoomStruct.heightObjects.end(), _heightObjects.begin());

	for (int i = 0; i < E_CAPSULE_HEIGHT_POOL_MAX_COUNT; i++)
	{
		if (_heightObjects[i] != nullptr)
		{
			_heightObjects[i]->SwitchOwnerId(_roomId);

			UINT16 objectCode = _heightObjects[i]->ObjectCode();
			if (objectCode == O_LOOSE_PIVOT)
			{
				LoosePivotRef loosePivot = static_pointer_cast<LoosePivot>(_heightObjects[i]);
				if (loosePivot->IsWobble())
				{
					DoTimer(200, &Room::AddressLoosePivot_AfterMergeFinished, (UINT8)i);
				}
			}
			else if (objectCode == O_STRONG_BIRD)
			{
				DoTimer(20, &Room::UpdateBirdPos, (UINT8)i, (UINT16)O_STRONG_BIRD);
			}
		}
	}

	_leastHeightIndex = 0;
	_maxHeightIndex = retrievingRoomStruct.maxHeightIndex;

	
	_multiRoom = multiRoom;
}

void CapsuleRoom::Handle_C_SPLIT_ROOM_FINISHED(ClientSessionRef client, BYTE* buffer, int32 len)
{
	(*_clients.begin())->SwitchRoom(static_pointer_cast<Room>(shared_from_this()));

	if (MultiRoomRef multiRoom = _multiRoom.lock())
	{
		multiRoom->DoAsync(&MultiRoom::OnCapsuleRoomTransferFinished, client);
	}

	_multiRoom.reset();


	client->SwitchRoom(static_pointer_cast<Room>(shared_from_this()));




	PlayerRef player = _players[(*_clients.begin())->PlayerId()];
	Vector2 playerPos = player->GetPos();

	_playersHighestPos = playerPos;
	_playersLowestPos = playerPos;


	FlatBufferReader fbr(buffer, len);
	UINT8 playerNewIndex = fbr.Read<UINT8>();
	if (playerNewIndex != 255)
	{
		player->SetPivotIndex(playerNewIndex);
	}
	else
	{
		
	}


	DoTimer(MATCH_SLEEP_TICK, &CapsuleRoom::StartMatch);
}


void CapsuleRoom::OnExitGame(ClientSessionRef client)
{
	if (MultiRoomRef multiRoom = _multiRoom.lock())
	{
		multiRoom->DoAsync(&MultiRoom::OnExitGame, client);
	}

	Room::OnExitGame(client);
}


MergingRoomStruct CapsuleRoom::MergeSource()
{
	return MergingRoomStruct(*_clients.begin(),
		_players[_roomId], _heightObjects, _maxHeightIndex, _leastHeightIndex, _roomId);
}
