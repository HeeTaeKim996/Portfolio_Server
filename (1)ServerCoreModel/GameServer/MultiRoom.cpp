#include "pch.h"
#include "MultiRoom.h"

#include "Room.h"
#include "CapsuleRoom.h"
#include "Pivot.h"
#include "BasePivot.h"
#include "LoosePivot.h"
#include "Player.h"

#include "FlatBufferWriter.h"
#include "FlatBufferReader.h"
#include "ClientSession.h"
#include "Lobby.h"

#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 
#include "HeightObject.h"

#include "Energy.h"
#include "BlueEnergy.h"


MultiRoom::MultiRoom(CapsuleRoomRef firstRoom, CapsuleRoomRef secondRoom)
	: Room((UINT64)0, E_MULTI_HEIGHT_POOL_MAX_COUNT)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddMultiRoom();
#endif 


	// TODO : When Player Exit In Merging.. Assert Crash. Address It


	MergingRoomStruct first = firstRoom->MergeSource();
	MergingRoomStruct second = secondRoom->MergeSource();


	Vector<HeightLog> logs;


	UINT32 averageScore = (UINT32)(first.player->Score() + second.player->Score());
	if (averageScore != 0)
	{
		averageScore %= 2;
	}

	firstMoveTowardLog.playerId = first.ownerId;
	secondMoveTowardLog.playerId = second.ownerId;
	LinkerPattern_A(logs, first, second, firstMoveTowardLog.moveTowardVec, 
		secondMoveTowardLog.moveTowardVec, averageScore);

	

	_players[first.ownerId] = first.player;
	_players[second.ownerId] = second.player;


	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_MERGE_ROOM);



		fbw.Write<UINT64>(first.ownerId); // OwnerId
		fbw.Write<Vector2>(firstMoveTowardLog.moveTowardVec); // World Move Toward
		fbw.Write<UINT64>(second.ownerId); // OwnerId
		fbw.Write<Vector2>(secondMoveTowardLog.moveTowardVec); // World Move Toward


		fbw.Write<UINT64>(first.ownerId); // OwnerId
		fbw.WriteWString(first.clientSession->NickName()); // NickName
		fbw.Write((UINT32)first.clientSession->BestScore()); // BestScore
		fbw.Write((UINT8)first.player->BlueEnergy()); // BlueEnergy

		fbw.Write<UINT16>(first.player->ObjectCode()); // ObjectCode
		fbw.Write<Vector2>(first.player->GetPos()); // Pos	
		fbw.Write<float>(first.player->GetRot()); // Rot


		fbw.Write<UINT64>(second.ownerId); // OwnerId
		fbw.WriteWString(second.clientSession->NickName()); // NickName
		fbw.Write((UINT32)second.clientSession->BestScore()); // BestScore
		fbw.Write((UINT8)second.player->BlueEnergy()); // BlueEnergy

		fbw.Write<UINT16>(second.player->ObjectCode()); // ObjectCode
		fbw.Write<Vector2>(second.player->GetPos()); // Pos
		fbw.Write<float>(second.player->GetRot()); // Rot



		fbw.Write<UINT8>(static_cast<UINT8>(logs.size())); // Pivots Vec Size
		for (const HeightLog& log : logs)
		{
			fbw.Write<UINT64>(log.smallHeightLog.ownerId); // Owner Id
			fbw.Write<UINT8>(log.smallHeightLog.previousIndex); // Previous Index


			if (log.smallHeightLog.movedHeightObject->Category() == HeightObject::CategoryType::Pivot)
			{
				fbw.Write<UINT8>(HECA_PIVOT); // Category
				fbw.Write<UINT16>(log.smallHeightLog.movedHeightObject->ObjectCode()); // ObjectCode
				fbw.Write<UINT8>(log.index); // new Index
				fbw.Write<Vector2>(log.smallHeightLog.movedHeightObject->GetPos()); // Moved Pos
				fbw.Write<float>(log.smallHeightLog.movedHeightObject->GetRot()); // Rot
			}
			else if (log.smallHeightLog.movedHeightObject->Category()
				== HeightObject::CategoryType::Energy)
			{
				fbw.Write<UINT8>(HECA_ENERGY); // Category
				fbw.Write<UINT16>(log.smallHeightLog.movedHeightObject->ObjectCode()); // ObjectCode
				fbw.Write<UINT8>(log.index); // new Index
				fbw.Write<Vector2>(log.smallHeightLog.movedHeightObject->GetPos()); // Moved Pos
				fbw.Write<float>(log.smallHeightLog.movedHeightObject->GetRot()); // Rot
				
				fbw.Write<UINT8>(static_pointer_cast<Energy>(log.smallHeightLog.movedHeightObject)
					->Amount()); // Amount
			}
			else
			{
				CRASH("NO_CATEGORY_TYPE");
			}
		}
	}

	first.clientSession->Send(sendBuffer);
	second.clientSession->Send(sendBuffer);
}

MultiRoom::~MultiRoom()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveMultiRoom();
#endif 

}

void MultiRoom::Multi_OnMergeRoomFinished(ClientSessionRef client, BYTE* buffer, int32 len)
{
	_capsuleRooms[client->PlayerId()] = static_pointer_cast<CapsuleRoom>(client->Room());
	client->SwitchRoom(static_pointer_cast<Room>(shared_from_this()));
	if (CapsuleRoomRef capsuleRoom = _capsuleRooms[client->PlayerId()].lock())
	{
		capsuleRoom->OnRoomTransferFinished();
	}
	_clients.insert(client);


	Vector2 moveTowardVec;
	if (firstMoveTowardLog.playerId == client->PlayerId())
	{
		moveTowardVec = firstMoveTowardLog.moveTowardVec;
	}
	else if (secondMoveTowardLog.playerId == client->PlayerId())
	{
		moveTowardVec = secondMoveTowardLog.moveTowardVec;
	}
	else
	{
		CRASH("NOT_IDENTIFIED_ID");
	}


	PlayerRef player = _players[client->PlayerId()];
	player->Update_HeightAndInterpolation(moveTowardVec.y);
	

	FlatBufferReader fbr(buffer, len);
	UINT8 playerNewIndex = fbr.Read<UINT8>();
	float xPos = fbr.Read<float>();
	float yPos = fbr.Read<float>();
	

	Vector2 newPos(xPos, yPos);

	if (_clients.size() == 1)
	{
		_playersHighestPos = newPos;
		_playersLowestPos = newPos;
	}
	else
	{
		if (_playersHighestPos.y < yPos)
		{
			_playersHighestPos = newPos;
		}
		else
		{
			_playersLowestPos = newPos;
		}
	}

	if (playerNewIndex != 255)
	{
		player->SetPivotIndex(playerNewIndex);
	}
	else
	{
		
	}

	
	if (_clients.size() == 2)
	{
		for (int i = 0; i < E_MULTI_HEIGHT_POOL_MAX_COUNT; i++)
		{
			if (_heightObjects[i] != nullptr)
			{
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
	}


}

void MultiRoom::OnExitGame(ClientSessionRef client)
{
	_clients.erase(client);

	auto it = _capsuleRooms.find(client->PlayerId());
	if (it != _capsuleRooms.end())
	{
		if (CapsuleRoomRef capsuleRoom = it->second.lock())
		{
			capsuleRoom->OnExitGame(client);
		}
	}


	ClientSessionRef remainClient = *_clients.begin();


	if (splitCause != SplitCause::DEFAULT)
	{
		return;
	}
	splitCause = SplitCause::PLAYER_EXIT;
	SplitRoom(remainClient);

}


void MultiRoom::LinkerPattern_A(Vector<HeightLog>& logs, MergingRoomStruct& first, MergingRoomStruct& second,
	Vector2& firstMoveToward, Vector2& secondMoveToward, UINT32 averageScore)
{
	MergePivots(logs, first, second, Vector2(-20.f, 0.f), Vector2(20.f, 1.f), firstMoveToward, secondMoveToward);

	for (int i = 0; i < 8; i++)
	{
		float horizontal;
		float vertical;
		if (i % 2 == 0)
		{ // First
			horizontal = -24.f + (i + 1) * 3.f;
			vertical = 1.f + (i + 1) * 3.f;

			CreatePivot_ForLinker(first.ownerId, logs, Vector2(horizontal, vertical), averageScore);
		}
		else
		{ // Second
			horizontal = 24.f - i * 3.f;
			vertical = 2.f + i * 3.F;

			CreatePivot_ForLinker(second.ownerId, logs, Vector2(horizontal, vertical), averageScore);
		}

	}

	CreatePivot_ForLinker(_roomId, logs, Vector2(0.f, 26.f), averageScore);

}

void MultiRoom::MergePivots(Vector<HeightLog>& logs, MergingRoomStruct& first, MergingRoomStruct& second,
	Vector2 firstAim, Vector2 secondAim, Vector2& firstMoveToward, Vector2& secondMoveToward)
{
	auto DeepCopyAndMove = [&](Vector<SmallHeightLog>& smallLogs, const Vector2& moveVec,
		MergingRoomStruct& source, UINT8 size)
		{
			UINT8 copyingIndex = source.leastHeightIndex;

			for (int i = 0; i < size; i++)
			{
				if (source.heightObjects[copyingIndex] != nullptr)
				{
					HeightObjectRef movedPivot = source.heightObjects[copyingIndex]->Clone();
					movedPivot->MoveToward(moveVec);

					SmallHeightLog smallLog{ movedPivot, source.heightObjects[copyingIndex]->Index(),
					source.ownerId };

					smallLogs.push_back(smallLog);
				}

				if (++copyingIndex >= E_CAPSULE_HEIGHT_POOL_MAX_COUNT)
				{
					copyingIndex = 0;
				}
			}
		};


	firstMoveToward = firstAim - first.heightObjects[first.maxHeightIndex]->GetPos();
	secondMoveToward = secondAim - second.heightObjects[second.maxHeightIndex]->GetPos();




	UINT8 firstSize;
	if (first.maxHeightIndex >= first.leastHeightIndex)
	{
		firstSize = first.maxHeightIndex - first.leastHeightIndex + 1;
	}
	else
	{
		firstSize = (first.maxHeightIndex + 1) + (E_CAPSULE_HEIGHT_POOL_MAX_COUNT - first.leastHeightIndex);
	}
	Vector<SmallHeightLog> firstSmallLogs;

	UINT8 secondSize;
	if (second.maxHeightIndex >= second.leastHeightIndex)
	{
		secondSize = second.maxHeightIndex - second.leastHeightIndex + 1;
	}
	else
	{
		secondSize = (second.maxHeightIndex + 1) + (E_CAPSULE_HEIGHT_POOL_MAX_COUNT - second.leastHeightIndex);
	}
	Vector<SmallHeightLog> secondSmallLogs;


	DeepCopyAndMove(firstSmallLogs, firstMoveToward, first, firstSize);
	DeepCopyAndMove(secondSmallLogs, secondMoveToward, second, secondSize);


	UINT8 firstIndex = 0;
	SmallHeightLog firstLog;
	bool isFirstLoggable = firstIndex < firstSmallLogs.size() ? true : false;
	if (isFirstLoggable)
	{
		firstLog = firstSmallLogs[firstIndex++];
	}

	UINT8 secondIndex = 0;
	SmallHeightLog secondLog;
	bool isSecondLoggable = secondIndex < secondSmallLogs.size() ? true : false;
	if (isSecondLoggable)
	{
		secondLog = secondSmallLogs[secondIndex++];
	}

	UINT8 newIndex = 0;

	while (isFirstLoggable || isSecondLoggable)
	{
		if (isFirstLoggable && isSecondLoggable)
		{
			if (firstLog.movedHeightObject->GetPos().y < secondLog.movedHeightObject->GetPos().y)
			{
				_heightObjects[newIndex] = firstLog.movedHeightObject;
				_heightObjects[newIndex]->SwitchIndex(newIndex);

				HeightLog log{ newIndex, firstLog };
				logs.push_back(log);

				if (isFirstLoggable = firstIndex < firstSmallLogs.size())
				{
					firstLog = firstSmallLogs[firstIndex++];
				}
			}
			else
			{
				_heightObjects[newIndex] = secondLog.movedHeightObject;
				_heightObjects[newIndex]->SwitchIndex(newIndex);

				HeightLog log{ newIndex, secondLog };
				logs.push_back(log);

				if (isSecondLoggable = secondIndex < secondSmallLogs.size())
				{
					secondLog = secondSmallLogs[secondIndex++];
				}
			}
		}
		else if (isFirstLoggable)
		{
			_heightObjects[newIndex] = firstLog.movedHeightObject;
			_heightObjects[newIndex]->SwitchIndex(newIndex);

			HeightLog log{ newIndex, firstLog };
			logs.push_back(log);

			if (isFirstLoggable = firstIndex < firstSmallLogs.size())
			{
				firstLog = firstSmallLogs[firstIndex++];
			}
		}
		else if (isSecondLoggable)
		{
			_heightObjects[newIndex] = secondLog.movedHeightObject;
			_heightObjects[newIndex]->SwitchIndex(newIndex);

			HeightLog log{ newIndex, secondLog };
			logs.push_back(log);

			if (isSecondLoggable = secondIndex < secondSmallLogs.size())
			{
				secondLog = secondSmallLogs[secondIndex++];
			}
		}

		newIndex++;
	}

	_leastHeightIndex = 0;
	_maxHeightIndex = --newIndex;
}

void MultiRoom::CreatePivot_ForLinker(UINT64 ownerId, Vector<HeightLog>& logs, Vector2 instanPos, 
	UINT32 averageScore)
{
	_maxHeightIndex++;

#ifdef _DEBUG
	if (_maxHeightIndex >= _heightObjectsSize)
	{
		CRASH("UPPER PIVOTS SIZE");
	}
#endif 
	HeightObjectRef newPivot = Room::CretaePivot(averageScore, instanPos, ownerId, _maxHeightIndex);

	_heightObjects[_maxHeightIndex] = newPivot;

	SmallHeightLog smallLog{ newPivot, 0, 0 };
	HeightLog log{ _maxHeightIndex, smallLog };
	logs.push_back(log);
}

CapsuleRoomRef MultiRoom::GetCapsuleRoom(UINT64 playerId)
{
	return _capsuleRooms[playerId].lock();
}

void MultiRoom::RemoveCapsuleRoom(UINT64 playerId)
{
	_capsuleRooms.erase(playerId);
}

void MultiRoom::Handle_REQUIRE_ATTACH_TO_PIVOT(ClientSessionRef client, BYTE* buffer, int32 len)
{
	Room::Handle_REQUIRE_ATTACH_TO_PIVOT(client, buffer, len);


	UINT64 playerId = client->PlayerId();
	UINT64 attachingPivotsOwnerId = _heightObjects[_players[playerId]->PivotIndex()]->OwnerId();


	if (attachingPivotsOwnerId != _roomId)
	{
		return;
	}


	if (_playersHighestPos.y - _playersLowestPos.y > 30.f
		|| std::abs(_playersHighestPos.x - _playersLowestPos.x) > 15.f)
	{
		if (splitCause != SplitCause::DEFAULT)
		{
			return;
		}
		splitCause = SplitCause::DISTANCE;

		for (ClientSessionRef client : _clients)
		{
			SplitRoom(client);
		}
	}
}

void MultiRoom::OnCapsuleRoomTransferFinished(ClientSessionRef client)
{
	_clients.erase(client);

	if (splitCause == SplitCause::PLAYER_EXIT)
	{
		GLobby->DoAsync(&Lobby::RemoveMultiRoom, static_pointer_cast<MultiRoom>(shared_from_this()));
	}
	else if (splitCause == SplitCause::DISTANCE)
	{
		if (_clients.size() <= 0)
		{
			GLobby->DoAsync(&Lobby::RemoveMultiRoom, static_pointer_cast<MultiRoom>(shared_from_this()));
		}
	}
}



void MultiRoom::SplitRoom(ClientSessionRef client)
{
	Vector2 playerPos = _heightObjects[_players[client->PlayerId()]->PivotIndex()]->GetPos();
	

	Vector<HeightObjectRef> transferPivots;
	transferPivots.resize(E_CAPSULE_HEIGHT_POOL_MAX_COUNT);
	Vector<RetrieveLog> logs;
	logs.resize(E_CAPSULE_HEIGHT_POOL_MAX_COUNT);


	UINT8 makingIndex = 0;
	UINT8 copyingIndex = _leastHeightIndex;

	UINT8 lastSearchIndex = _maxHeightIndex + 1;
	if (lastSearchIndex >= E_MULTI_HEIGHT_POOL_MAX_COUNT)
	{
		lastSearchIndex = 0;
	}

	while (makingIndex < E_CAPSULE_HEIGHT_POOL_MAX_COUNT && copyingIndex != lastSearchIndex)
	{
		if (_heightObjects[copyingIndex] != nullptr)
		{
			UINT64 pivotId = _heightObjects[copyingIndex]->OwnerId();
			if (pivotId == _roomId || pivotId == client->PlayerId())
			{
				Vector2 pivotPos = _heightObjects[copyingIndex]->GetPos();

				if (pivotPos.y  < playerPos.y + Room::MAKING_PIVOT_MIN_FLOAT
					&& pivotPos.y > playerPos.y - Room::DELETING_PIVOT_MAX_FLOAT
					&& std::abs(playerPos.x - pivotPos.x < 8.f))
				{
					transferPivots[makingIndex] = _heightObjects[copyingIndex]->Clone();
					transferPivots[makingIndex]->SwitchIndex(makingIndex);

					RetrieveLog log{ copyingIndex, makingIndex };
					logs[makingIndex] = log;

					makingIndex++;
				}
			}
		}


		if (++copyingIndex >= E_MULTI_HEIGHT_POOL_MAX_COUNT)
		{
			copyingIndex = 0;
		}
	}

	if (transferPivots[--makingIndex]->Category() == HeightObject::CategoryType::Energy)
	{
		transferPivots.pop_back();
		logs.pop_back();
		makingIndex--;
	}


	RetrievingRoomStruct retrieveRoomStruct(transferPivots, makingIndex);

	if (CapsuleRoomRef capsuleRoom = _capsuleRooms[client->PlayerId()].lock())
	{
		capsuleRoom->RetrievePlayer(retrieveRoomStruct, static_pointer_cast<MultiRoom>(shared_from_this()));
		
	}

	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_SPLIT_ROOM);
		fbw.Write<UINT8>(++makingIndex); // Pivot Length
		for (RetrieveLog log : logs)
		{
			fbw.Write<UINT8>(log.previousIndex); // Previous Index
			fbw.Write<UINT8>(log.newIndex); // New Index
		}
	}

	client->Send(sendBuffer);
}