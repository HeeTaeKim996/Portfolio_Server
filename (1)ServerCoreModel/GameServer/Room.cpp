#include "pch.h"
#include "Room.h"
#include "ClientSession.h"
#include "Lobby.h"

#include "FlatBufferWriter.h"
#include "FlatBufferReader.h"

#include <string>

#include "Player.h"
#include "Pivot.h"
#include "BasePivot.h"
#include "SlickPivot.h"
#include "LoosePivot.h"
#include "StrongBird.h"

#include "BlueEnergy.h"

#include "HeightObject.h"

Room::Room(uint64 roomId, UINT8 pivotsSize)
	: _roomId(roomId), _heightObjectsSize(pivotsSize)
{

	_playersHighestPos = Vector2(0, FLT_MIN);
	_playersLowestPos = Vector2(0, FLT_MAX);


	std::random_device rd;
	_rng.seed(rd());

	hundredDist = std::uniform_int_distribution<int>(1, 100);

	_pivotDistSets.resize(3);
	_pivotDistSets[0] = { std::uniform_int_distribution<int>(-40, 40),
	std::uniform_int_distribution<int>(45, 55) };
	_pivotDistSets[1] = { std::uniform_int_distribution<int>(-50, 50),
	std::uniform_int_distribution<int>(55, 60) };
	_pivotDistSets[2] = { std::uniform_int_distribution<int>(-60, 60),
	std::uniform_int_distribution<int>(60, 65) };



	_heightObjects.resize(pivotsSize);

	HeightObjectRef pivot = ObjectPool<BasePivot>::MakeShared(roomId, _maxHeightIndex,
		Vector2(-1.35f, 0.f), 0.f);
	
	_heightObjects[_maxHeightIndex] = pivot;
	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_SPAWN_HEIGHT_OBJECT);

		fbw.Write((UINT8)HECA_PIVOT); // Category
		fbw.Write((UINT16)pivot->ObjectCode()); // ObjectCode
		fbw.Write((UINT8)_maxHeightIndex); // Index
		fbw.Write((Vector2)pivot->GetPos()); // pos
		fbw.Write((FLOAT)pivot->GetRot()); // Rot
	}
	CastAll(sendBuffer);


	_leastHeightIndex = _maxHeightIndex;


	_energyQuota = 4;
	_energyDist = std::uniform_int_distribution<int>(4, 12);

	CreateHeightObject(pivot->GetPos().y, 0);
}

Room::~Room()
{
#ifdef _DEBUG
	//cout << "Room Removed Check" << endl;
#endif 

}


void Room::Enter(ClientSessionRef client)
{
	_clients.insert(client);
	client->EnterRoom(static_pointer_cast<Room>(shared_from_this()));

	{
		UINT64 playerId = client->PlayerId();

		PlayerRef newPlayer = ObjectPool<Player>::MakeShared(playerId, Vector2(-1.35f, 2.34f), 0.f,
			O_Player, client);
		_players[playerId] = newPlayer;
		client->SetPlayerObject(newPlayer);



		/* Multi Room */
		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_INSTANTIATE_PLAYER);
			fbw.Write((uint64)playerId); // owners Id
			fbw.Write((UINT16)newPlayer->ObjectCode()); // NetObjectCode
			fbw.Write((Vector2)newPlayer->GetPos()); // Pos
			fbw.Write((float)newPlayer->GetRot()); // Rot
		}
		CastOthers(client, sendBuffer);
	}

	InformRoomInfos();
	/* Multi Room  */
}

void Room::OnExitGame(ClientSessionRef client)
{
	_clients.erase(client);
	client->OnExitRoom();

	if (_clients.size() <= 0)
	{
		GLobby->DoAsync(&Lobby::RemoveCapsuleRoom, _roomId);
	}
	else
	{
		InformRoomInfos();
		_players.erase(client->PlayerId());
	}

}

void Room::BackToLobby(ClientSessionRef client)
{
	OnExitGame(client);

	GLobby->DoAsync(&Lobby::AcceptClient, client);
}



void Room::OnEnterRoomFinished(ClientSessionRef client, BYTE* buffer, int32 len)
{
	LoadLevel(client);
}




void Room::LoadLevel(ClientSessionRef welcomedClient)
{
	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_LOAD_LEVEL);


		// Players
		fbw.Write((UINT8)_players.size()); // PlayerCount
		for (const pair<UINT64, PlayerRef>& map : _players)
		{
			fbw.Write((UINT64)map.first); // owner Id
			fbw.Write((UINT16)O_Player); // NetObjectcode
			fbw.Write((Vector2)map.second->GetPos()); // Pos
			fbw.Write((float)map.second->GetRot()); // Rot
		}


		// Pivots
		if (_maxHeightIndex >= _leastHeightIndex)
		{
			UINT8 size = _maxHeightIndex - _leastHeightIndex + 1;

			fbw.Write((UINT8)size); // Size
			for (int i = _leastHeightIndex; i <= _maxHeightIndex; i++)
			{
				if (_heightObjects[i] != nullptr)
				{
					fbw.Write((UINT16)_heightObjects[i]->ObjectCode()); // ObjectCode
					fbw.Write((UINT8)i); // Index
					fbw.Write((Vector2)_heightObjects[i]->GetPos()); // pos
					fbw.Write((FLOAT)_heightObjects[i]->GetRot()); // Rot
				}
			}
		}
		else
		{
			UINT8 size = (_heightObjectsSize - _leastHeightIndex) + (_maxHeightIndex + 1);

			fbw.Write((UINT8)size); // Size
			for (int i = _leastHeightIndex; i < _heightObjectsSize; i++)
			{
				if (_heightObjects[i] != nullptr)
				{
					fbw.Write((UINT16)_heightObjects[i]->ObjectCode()); // ObjectCode
					fbw.Write((UINT8)i); // Index
					fbw.Write((Vector2)_heightObjects[i]->GetPos()); // pos
					fbw.Write((FLOAT)_heightObjects[i]->GetRot()); // Rot
				}
			}

			for (int i = 0; i <= _maxHeightIndex; i++)
			{
				if (_heightObjects[i] != nullptr)
				{
					fbw.Write((UINT16)_heightObjects[i]->ObjectCode()); // ObjectCode
					fbw.Write((UINT8)i); // Index
					fbw.Write((Vector2)_heightObjects[i]->GetPos()); // pos
					fbw.Write((FLOAT)_heightObjects[i]->GetRot()); // Rot
				}
			}
		}
	}

	welcomedClient->Send(sendBuffer);
}


/*----------------------
	  Room Utils
----------------------*/
void Room::InformRoomInfos()
{
	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_INFORM_ROOM_INFOS);

		fbw.Write(static_cast<uint32>(_clients.size()));
	}

	CastAll(sendBuffer);
}


void Room::CastAll(SendBufferRef sendBuffer)
{
	for (ClientSessionRef client : _clients)
	{
		client->Send(sendBuffer);
	}
}

void Room::CastOthers(ClientSessionRef exceptClient, SendBufferRef sendBuffer)
{
	for (ClientSessionRef client : _clients)
	{
		if (client == exceptClient)
		{
			continue;
		}
		client->Send(sendBuffer);
	}
}

void Room::CastOnly(ClientSessionRef onlyClient, SendBufferRef sendBuffer)
{
	onlyClient->Send(sendBuffer);
}





/*-------------------
	 HeightObject
-------------------*/
void Room::CreateHeightObject(float currentHeight, UINT32 score)
{

	if (--_energyQuota <= 0)
	{
		CreateEnergyObject(score);
		_energyQuota = _energyDist(_rng);

		return;
	}

	while (_heightObjects[_maxHeightIndex]->GetPos().y < currentHeight + MAKING_PIVOT_MIN_FLOAT)
	{
		PivotRef newPivot = CreatePivot_PrePos(score, _heightObjects[_maxHeightIndex]->GetPos());
		_heightObjects[_maxHeightIndex] = newPivot;

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_SPAWN_HEIGHT_OBJECT);

			fbw.Write((UINT8)HECA_PIVOT); // Category
			fbw.Write((UINT16)newPivot->ObjectCode()); // ObjectCode
			fbw.Write((UINT8)_maxHeightIndex); // Index
			fbw.Write((Vector2)newPivot->GetPos()); // pos
			fbw.Write((FLOAT)newPivot->GetRot()); // Rot
		}
		CastAll(sendBuffer);
	}
}



void Room::DeleteHeightObject(float currentHeight)
{

	while (true)
	{
		if (_heightObjects[_leastHeightIndex] == nullptr)
		{
			if (++_leastHeightIndex >= _heightObjectsSize)
			{
				_leastHeightIndex = 0;
			}
			continue;
		}

		if (_heightObjects[_leastHeightIndex]->GetPos().y >= currentHeight - DELETING_PIVOT_MAX_FLOAT)
		{
			break;
		}


		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_DELETE_HEIGHT_OBJECT);
			fbw.Write<UINT8>(_leastHeightIndex);
		}
		CastAll(sendBuffer);



		_heightObjects[_leastHeightIndex] = nullptr;

		if (++_leastHeightIndex >= _heightObjectsSize)
		{
			_leastHeightIndex = 0;
		}
	}
}

void Room::CreateEnergyObject(UINT32 score)
{
	if (_heightObjects[_maxHeightIndex]->Category() != HeightObject::CategoryType::Pivot)
	{
		CRASH("NOT_PIVOT");
	}

	Vector<Vector2> vectors;
	vectors.resize(5);



	vectors[0] = _heightObjects[_maxHeightIndex]->GetPos();
	vectors[1] = CreateVector2_PrePos(score, vectors[0]);
	vectors[2] = CreateVector2_PrePos(score, vectors[1]);
	vectors[3] = CreateVector2_PrePos(score, vectors[2]);
	vectors[4] = CreateVector2_PrePos(score, vectors[3]);


	UINT8 randMax;
	float maxDistance = 60.f;

	Vector2 firstPivotPos = vectors[0];
	if (Vector2::Distance(firstPivotPos, vectors[4]) < maxDistance)
	{
		randMax = 4;
	}
	else if (Vector2::Distance(firstPivotPos, vectors[3]) < maxDistance)
	{
		randMax = 3;
	}
	else if (Vector2::Distance(firstPivotPos, vectors[2]) < maxDistance)
	{
		randMax = 2;
	}
	else if (Vector2::Distance(firstPivotPos, vectors[1]) < maxDistance)
	{
		randMax = 1;
	}
	else
	{
		CRASH("NO_DISTANCE");
	}


	UINT8 rand;
	if (randMax > 1)
	{
		rand = std::uniform_int_distribution<int>(1, randMax)(_rng);
	}
	else
	{
		rand = 1;
	}

	Vector2 eneryInstanPos = Vector2::BetweenPoint(firstPivotPos, vectors[rand], 50);

	UINT8 recordedStartIndex = _maxHeightIndex;

	for (int i = 1; i <= 4; i++)
	{
		if (++_maxHeightIndex >= _heightObjectsSize)
		{
			_maxHeightIndex = 0;
		}
#ifdef _DEBUG
		if (_heightObjects[_maxHeightIndex] != nullptr)
		{
			CRASH("Head Eats Tail");
		}
#endif 


		if (i == rand)
		{
			_heightObjects[_maxHeightIndex] = 
				ObjectPool<BlueEnergy>::MakeShared(_roomId, eneryInstanPos, 0.f, O_BLUE_ENERGY, 
					_maxHeightIndex, 50);

			if (++_maxHeightIndex >= _heightObjectsSize)
			{
				_maxHeightIndex = 0;
			}
#ifdef _DEBUG
			if (_heightObjects[_maxHeightIndex] != nullptr)
			{
				CRASH("Head Eats Tail");
			}
#endif 

			_heightObjects[_maxHeightIndex] = 
				CretaePivot(score, vectors[i], _roomId, _maxHeightIndex);
		}
		else
		{
			_heightObjects[_maxHeightIndex] =
				CretaePivot(score, vectors[i], _roomId, _maxHeightIndex);
		}
	}


	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_SPAWN_HEIGHT_OBJECTS_INCLUDE_ENERGY);

		// Energy
		UINT8 energyIndex = recordedStartIndex + rand;
		if (energyIndex >= _heightObjects.size())
		{
			energyIndex -= _heightObjects.size();
		}

		EnergyRef energy = static_pointer_cast<Energy>(_heightObjects[energyIndex]);
		fbw.Write((UINT16)energy->ObjectCode()); // ObjectCode
		fbw.Write((UINT8)energy->Index()); // Index
		fbw.Write((Vector2)energy->GetPos()); // Pos
		fbw.Write((float)energy->GetRot()); // Rot
		fbw.Write((UINT8)energy->Amount()); // Amount

		// Pivot
		UINT8 pivotIndex = recordedStartIndex;


		for (int i = 1; i <= 4; i++)
		{
			if (i == rand)
			{
				pivotIndex += 2;
				
			}
			else
			{
				pivotIndex++;
			}
			if (pivotIndex >= _heightObjects.size())
			{
				pivotIndex -= _heightObjects.size();
			}

			PivotRef pivot = static_pointer_cast<Pivot>(_heightObjects[pivotIndex]);

			fbw.Write((UINT16)pivot->ObjectCode()); // Object Code
			fbw.Write((UINT8)pivot->Index()); // Index
			fbw.Write((Vector2)pivot->GetPos()); // Pos
			fbw.Write((float)pivot->GetRot()); // Rot
		}
	}
	CastAll(sendBuffer);
}



/*---------------
	  Pivot
---------------*/
PivotRef Room::CreatePivot_PrePos(UINT32 score, Vector2 prePos)
{
	if (++_maxHeightIndex >= _heightObjectsSize)
	{
		_maxHeightIndex = 0;
	}
	return CretaePivot(score, CreateVector2_PrePos(score, prePos), _roomId, _maxHeightIndex);
}

Vector2 Room::CreateVector2_PrePos(UINT32 score, Vector2 prePos)
{
	Vector2 instanPos;


	if (score < 200)
	{
		instanPos = Vector2(prePos.x +
			static_cast<float>(_pivotDistSets[0].distX(_rng)) / 10.f,
			prePos.y + static_cast<float>(_pivotDistSets[0].distY(_rng)) / 10.f);
	}
	//else if (score < 400)
	else
	{
		instanPos = Vector2(prePos.x +
			static_cast<float>(_pivotDistSets[1].distX(_rng)) / 10.f,
			prePos.y + static_cast<float>(_pivotDistSets[1].distY(_rng)) / 10.f);
	}
#if 0
	else
	{
		instanPos = Vector2(prePos.x +
			static_cast<float>(_pivotDistSets[2].distX(_rng)) / 10.f,
			prePos.y + static_cast<float>(_pivotDistSets[2].distY(_rng)) / 10.f);
	}
#endif
	return instanPos;
}

PivotRef Room::CretaePivot(UINT32 score, Vector2 instanPos, UINT64 ownerId, UINT8 index)
{
	PivotRef newPivot;

	int hundredValue = hundredDist(_rng);

	if (score < 5)
	{
		newPivot = ObjectPool<BasePivot>::MakeShared(ownerId, index, instanPos, 0.f);
	}
	else if (score < 600)
	{
		if (hundredValue <= 5)
		{
			newPivot = ObjectPool<BasePivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if (hundredValue <= 10)
		{
			newPivot = ObjectPool<SlickPivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if(hundredValue <= 90)
		{
			newPivot = ObjectPool<LoosePivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if (hundredValue <= 100)
		{
			float randMaxMoveDistance = float(uniform_int_distribution<int>(20, 40)(_rng))
				/ 10.f;
			float randMovementSpeed = float(uniform_int_distribution<int>(15, 30)(_rng)) 
				/ 10'000.f;
			
			newPivot = ObjectPool<StrongBird>::MakeShared(ownerId, index, instanPos, 0.f, 
				randMaxMoveDistance, randMovementSpeed);
			DoTimer(20, &Room::UpdateBirdPos, index, (UINT16)O_STRONG_BIRD);
		}
	}
	else if (score < 400)
	{
		if (hundredValue <= 0)
		{
			newPivot = ObjectPool<BasePivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if (hundredValue <= 0)
		{
			newPivot = ObjectPool<SlickPivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if(hundredValue <= 0)
		{
			newPivot = ObjectPool<LoosePivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if (hundredValue <= 100)
		{
			float randMaxMoveDistance = float(uniform_int_distribution<int>(20, 40)(_rng))
				/ 10.f;
			float randMovementSpeed = float(uniform_int_distribution<int>(15, 30)(_rng))
				/ 10'000.f;

			newPivot = ObjectPool<StrongBird>::MakeShared(ownerId, index, instanPos, 0.f,
				randMaxMoveDistance, randMovementSpeed);
			DoTimer(20, &Room::UpdateBirdPos, index, (UINT16)O_STRONG_BIRD);
		}
	}
	else
	{
		if (hundredValue <= 0)
		{
			newPivot = ObjectPool<BasePivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if (hundredValue <= 0)
		{
			newPivot = ObjectPool<SlickPivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if(hundredValue <= 0)
		{
			newPivot = ObjectPool<LoosePivot>::MakeShared(ownerId, index, instanPos, 0.f);
		}
		else if (hundredValue <= 100)
		{
			float randMaxMoveDistance = float(uniform_int_distribution<int>(20, 40)(_rng))
				/ 10.f;
			float randMovementSpeed = float(uniform_int_distribution<int>(15, 30)(_rng))
				/ 10'000.f;

			newPivot = ObjectPool<StrongBird>::MakeShared(ownerId, index, instanPos, 0.f,
				randMaxMoveDistance, randMovementSpeed);
			DoTimer(20, &Room::UpdateBirdPos, index, (UINT16)O_STRONG_BIRD);
		}
	}



	return newPivot;
}







void Room::DeleteLoosePivotAfterTime(UINT8 index)
{
	if (_heightObjects[index] != nullptr && _heightObjects[index]->ObjectCode() == O_LOOSE_PIVOT)
	{
		LoosePivotRef loosePivot = static_pointer_cast<LoosePivot>(_heightObjects[index]);

		if (!loosePivot->IsWobble())
		{
			return;
		}


		_heightObjects[index] = nullptr;




		Vector<std::pair<UINT64, UINT8>> droppingPlayerLog;
		for (std::pair<UINT64, PlayerRef> pair : _players)
		{
			if (pair.second->PivotIndex() == index)
			{
				UINT8 playerState = pair.second->PlayerState();
				if (playerState == PS_Idle || playerState == PS_Swing)
				{
					UINT8 afterPlayerState = playerState == PS_Idle ? PS_FALLING : PS_Jumping;
					pair.second->SwitchPlayerState(afterPlayerState);
					droppingPlayerLog.push_back(std::make_pair(pair.second->OwnerId(),
						afterPlayerState));
				}
			}
		}

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_LOOSE_PIVOT_DELETE);
			fbw.Write<UINT8>(index); // Index

			fbw.Write<UINT8>((UINT8)droppingPlayerLog.size()); // Dropping Player Count
			for (std::pair<UINT64, UINT8> dropLog : droppingPlayerLog)
			{
				fbw.Write<UINT64>(dropLog.first); // Dropping Player Id
				fbw.Write<UINT8>(dropLog.second); // Dropping Player State
			}
		}

		CastAll(sendBuffer);
	}
}

void Room::AddressLoosePivot_AfterMergeFinished(UINT8 index)
{
	if (LoosePivotRef loosePivot = static_pointer_cast<LoosePivot>(_heightObjects[index]))
	{
		UINT64 deleteTickTime = loosePivot->DeleteTickTime();
		if (GetTickCount64() >= deleteTickTime)
		{
			DeleteLoosePivotAfterTime(index);
		}
		else
		{
			UINT64 remainTime = deleteTickTime - GetTickCount64();
			DoTimer(remainTime, &Room::DeleteLoosePivotAfterTime, index);
		}
	}
}

/*----------------
	  Energy
----------------*/
void Room::Handle_REQUIRE_ENERGY(ClientSessionRef client, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);

	UINT8 index = fbr.Read<UINT8>();

	if (EnergyRef energy = dynamic_pointer_cast<Energy>(_heightObjects[index]))
	{
		Energy::EnergyType energyType = energy->GetEnergyType();
		UINT64 playerId = client->PlayerId();
		UINT8 afterEnergyAmount;

		if (energyType == Energy::EnergyType::BlueEnergy)
		{
			afterEnergyAmount = _players[playerId]->AddBlueEnergy(energy->Amount());
		}
		else
		{
			CRASH("NO_ENERGY_TYPE");
		}



		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_ACQUIRE_ENERGY);
			fbw.Write<UINT8>(index); // Index
			fbw.Write<UINT8>(energyType); // EnergyType
			fbw.Write<UINT64>(playerId); // PlayerId
			fbw.Write<UINT8>(afterEnergyAmount); // AfterEnergyAmount
		}

		CastAll(sendBuffer);
	}

	_heightObjects[index] = nullptr;
}

void Room::UpdateBirdPos(UINT8 index, UINT16 objectCode)
{
	if (_heightObjects[index] == nullptr)
	{
		return;
	}

	UINT16 indexesObjectCode = _heightObjects[index]->ObjectCode();
	
	Vector2 updatedPos;
	if (indexesObjectCode == O_STRONG_BIRD)
	{
		updatedPos = static_pointer_cast<StrongBird>(_heightObjects[index])->UpdatePos();
	}
	else
	{
		return;
		updatedPos = Vector2(0, 0);
	}
	


	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_UPDATE_BIRD_POS);
		fbw.Write<UINT8>(index); // Pivot Index
		fbw.Write<Vector2>(updatedPos); // Pos
	}

	CastAll(sendBuffer);

	DoTimer(20, &Room::UpdateBirdPos, index, objectCode);
}

/*---------------
	  Player
---------------*/
void Room::UpdatePlayerRotPos(ClientSessionRef owner, BYTE* buffer, int32 len)
{

	FlatBufferReader fbr(buffer, len);

	float x = fbr.Read<float>();
	float y = fbr.Read<float>();

	float playerRot = fbr.Read<float>();

	UINT64 playerId = owner->PlayerId();

	if (_players.find(playerId) == _players.end())
	{
		return;
	}

	_players[playerId]->SetPos(Vector2(x, y));
	_players[playerId]->SetRot(playerRot);


	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_PLAYER_UPDATE_ROT_POS);
		fbw.Write((UINT64)playerId); // NetId
		fbw.Write((float)x); // posX
		fbw.Write((float)y); // posY
		fbw.Write((float)playerRot); // rot
	}

	CastOthers(owner, sendBuffer);
}

void Room::Handle_ANNOUNCE_JUMP(ClientSessionRef client, BYTE* buffer, int32 len)
{
	UINT64 playerId = client->PlayerId();
	PlayerRef player = _players[playerId];


	player->SwitchPlayerState(PS_Jumping);


	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_ACQUIRE_JUMP);

		fbw.Write<UINT64>(playerId);
	}

	CastAll(sendBuffer);
}

void Room::Handle_REQUIRE_ATTACH_TO_PIVOT(ClientSessionRef client, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);
	UINT8 attachingPivotsIndex = fbr.Read<UINT8>();

	UINT64 playerId = client->PlayerId();

	PlayerRef player = _players[playerId];
	player->SetPivotIndex(attachingPivotsIndex);



	if (player->PlayerState() == PS_Jumping)
	{
		cout << "Room.cpp__DEBUG : Require Attach Index Is : " << (int)attachingPivotsIndex
			<< endl;

		{ // DEBUG
			if (_heightObjects[attachingPivotsIndex] == nullptr)
			{
				CRASH("WHY_NULL ?");
			}
		}

		float attachingHeight = _heightObjects[attachingPivotsIndex]->GetPos().y;
		if (player->BestHeight() < attachingHeight)
		{
			player->UpdateScore(attachingHeight);
		}
	}



	player->SwitchPlayerState(PS_Idle);



	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_ACQUIRE_ATTACH_TO_PIVOT);
		fbw.Write((UINT64)playerId); // Owner Id
		fbw.Write((UINT8)attachingPivotsIndex); // pivot index 
		fbw.Write((UINT32)player->Score()); // Score
		fbw.Write((float)PLAYER_DIE_FLOAT);
	}
	CastAll(sendBuffer);


	if (_heightObjects[attachingPivotsIndex] != nullptr
		&& _heightObjects[attachingPivotsIndex]->ObjectCode() == O_LOOSE_PIVOT)
	{
		if (LoosePivotRef loosePivot = static_pointer_cast<LoosePivot>
			(_heightObjects[attachingPivotsIndex]))
		{
			if (!loosePivot->IsWobble())
			{
				loosePivot->MakeWobble(1'500);
				DoTimer(1'500, &Room::DeleteLoosePivotAfterTime, attachingPivotsIndex);
			}

			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, S_LOOSE_PIVOT_WOBBLE);
				fbw.Write<UINT8>(attachingPivotsIndex);
			}

			CastAll(sendBuffer);
		}
	}







	float attachingHeight = _heightObjects[attachingPivotsIndex]->GetPos().y;
	if (attachingHeight > _playersHighestPos.y)
	{
		_playersHighestPos = _heightObjects[attachingPivotsIndex]->GetPos();
		CreateHeightObject(attachingHeight, player->Score());
	}


	float lowestPos = FLT_MAX;
	for (const pair<UINT64, PlayerRef>& player : _players)
	{
		UINT8 index = player.second->PivotIndex();
		if (_heightObjects[index] == nullptr) continue;

		Vector2 playerPos = _heightObjects[index]->GetPos();
		if (lowestPos > playerPos.y)
		{
			lowestPos = playerPos.y;
			_playersLowestPos = playerPos;
		}
	}

	DeleteHeightObject(_playersLowestPos.y);
}

void Room::Handle_PLAYER_DIE_TRIGGERED(ClientSessionRef client, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);
	UINT64 dieOwnerId = fbr.Read<UINT64>();

	if (_players.find(dieOwnerId) == _players.end())
	{
		return;
	}

	PlayerRef dyingPlayer = _players[dieOwnerId];

	if (ClientSessionRef clientSession = dyingPlayer->Client())
	{
		clientSession->UpdateScore(dyingPlayer->Score());
	}


	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_PLAYER_GAME_OVER);
		fbw.Write<UINT64>(dieOwnerId);
		fbw.Write<UINT32>(dyingPlayer->Score());
	}
	CastAll(sendBuffer);

	_players.erase(dieOwnerId);
}

void Room::Handle_PLAYER_FLIP(ClientSessionRef client, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);

	UINT8 isFlip = fbr.Read<UINT8>();
	// TODO?

	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_PLAYER_FLIP);
		fbw.Write((UINT64)client->PlayerId());
		fbw.Write((UINT8)isFlip);
	}

	CastOthers(client, sendBuffer);
}

void Room::Handle_REQUIRE_BLUE_SELF_RACKET(ClientSessionRef client, BYTE* buffer, int32 len)
{
	UINT64 playerId = client->PlayerId();
	PlayerRef player = _players[playerId];


	if (player->BlueEnergy() < 50)
	{
		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_DENY_BLUE_RACKET);
			fbw.Write<UINT64>(playerId); // PlayerID
			fbw.Write<UINT8>(player->BlueEnergy()); // Blue Amount;
		}

		CastOnly(client, sendBuffer);
	}
	else
	{
		FlatBufferReader fbr(buffer, len);
		float posX = fbr.Read<float>();
		float posY = fbr.Read<float>();
		float radToPivot = fbr.Read<float>();

		

		UINT8 remainBlueAmount = player->RemoveBlueEnergy(50);

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_ACQUIRE_BLUE_SELF_RACKET);

			fbw.Write<UINT64>(playerId); // PlayerId
			fbw.Write<UINT8>(remainBlueAmount); // Remain Blue Amount

			fbw.Write<float>(posX); // posX
			fbw.Write<float>(posY); // posY
			fbw.Write<float>(radToPivot); // Rad To Pivot
		}

		CastAll(sendBuffer);

		player->SwitchPlayerState(PS_Jumping);
	}

}

void Room::Handle_SYNC_PLAYER_ANIMATION(ClientSessionRef client, BYTE* buffer, int32 len)
{
	UINT64 playerId = client->PlayerId();

	FlatBufferReader fbr(buffer, len);
	Byte animationName = fbr.Read<Byte>();

	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_SYNC_PLAYER_ANIMATION);
		fbw.Write<UINT64>(playerId);
		fbw.Write<Byte>(animationName);
	}

	CastOthers(client, sendBuffer);
}

void Room::Handle_BLUE_SIEZE_MODE(ClientSessionRef client, BYTE* buffer, int32 len)
{
	UINT64 playerId = client->PlayerId();
	PlayerRef player = _players[playerId];

	if (player->PlayerState() == PS_Idle)
	{
		player->SwitchPlayerState(PS_BLUE_SIEGE_MODE);

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_BLUE_SIEZE_MODE);
			fbw.Write<UINT64>(playerId); // PlayerID
			fbw.Write<UINT8>(player->PivotIndex()); // AttachedPivotIndex
		}

		CastAll(sendBuffer);
	}
}

void Room::Handle_CANCEL_BLUE_SIEZE_MODE(ClientSessionRef client, BYTE* buffer, int32 len)
{
	UINT64 playerId = client->PlayerId();
	PlayerRef player = _players[playerId];

	if (player->PlayerState() == PS_BLUE_SIEGE_MODE)
	{
		player->SwitchPlayerState(PS_Idle);

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_ACQUIRE_CANCEL_SIEZE_MODE);
			fbw.Write<UINT64>(playerId);
		}

		CastAll(sendBuffer);
	}
}

void Room::Handle_SIEGE_MODE_FIRE(ClientSessionRef client, BYTE* buffer, int32 len)
{
	UINT64 flyingPlayerId = client->PlayerId();

	FlatBufferReader fbr(buffer, len);
	UINT64 siegingPlayerId = fbr.Read<UINT64>();

	if (_players[siegingPlayerId]->PlayerState() == PS_BLUE_SIEGE_MODE
		&& _players[siegingPlayerId]->BlueEnergy() >= 25
		&& _players[flyingPlayerId]->PlayerState() == PS_Idle)
	{
		_players[siegingPlayerId]->SwitchPlayerState(PS_BLUE_SIEGE_FIRING);
		_players[flyingPlayerId]->SwitchPlayerState(PS_BLUE_SIEGE_FIRING);

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_BLUE_SIEZE_MODE_FIRING);
			fbw.Write<UINT64>(siegingPlayerId); // siegingPlayerID
			fbw.Write<UINT64>(flyingPlayerId); // FlyingPlayerId
		}

		CastAll(sendBuffer);

		DoTimer(1500, &Room::SIEGE_MODE_FIRE, siegingPlayerId, flyingPlayerId);
	}
	else
	{
		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_DENIED_SIEGE_FIRING);
			fbw.Write<UINT64>(siegingPlayerId); // siegingPlayerID
			fbw.Write<UINT64>(flyingPlayerId); // FlyingPlayerId
		}
		CastAll(sendBuffer);
	}

}

void Room::SIEGE_MODE_FIRE(UINT64 siegingPlayerId, UINT64 flyingPlayerId)
{
	if (_players[siegingPlayerId]->PlayerState() == PS_BLUE_SIEGE_FIRING
		&& _players[siegingPlayerId]->BlueEnergy() >= 25
		&& _players[flyingPlayerId]->PlayerState() == PS_BLUE_SIEGE_FIRING)
	{
		_players[siegingPlayerId]->SwitchPlayerState(PS_Idle);
		UINT8 remainBlueAmount = _players[siegingPlayerId]->RemoveBlueEnergy(25);

		_players[flyingPlayerId]->SwitchPlayerState(PS_Jumping);

		SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
		{
			FlatBufferWriter fbw(sendBuffer, S_BLUE_SEIGE_MODE_fIRE);
			fbw.Write<UINT64>(siegingPlayerId); // siegingPlayerID
			fbw.Write<UINT8>(remainBlueAmount); // Remain Blue Amount

			fbw.Write<UINT64>(flyingPlayerId); // FlyingPlayerId
		}
		CastAll(sendBuffer);

	}
	else
	{
		CRASH("WHY");
	}
}

void Room::Handle_ANNOUNCE_SWING(ClientSessionRef client, BYTE* buffer, int32 len)
{
	PlayerRef player = _players[client->PlayerId()];

	if (player->PlayerState() == PS_Idle)
	{
		player->SwitchPlayerState(PS_Swing);
	}
	else
	{
		CRASH("NOT_IDLE");
	}
}

void Room::Handle_ANNOUNCE_IDLE(ClientSessionRef client, BYTE* buffer, int32 len)
{
	PlayerRef player = _players[client->PlayerId()];

	player->SwitchPlayerState(PS_Idle);
}






