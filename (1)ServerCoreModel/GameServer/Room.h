#pragma once

#include "JobQueue.h"
#include "NetObject.h"
#include <random>

class Room : public JobQueue
{
protected:
	static constexpr float MAKING_PIVOT_MIN_FLOAT = 20.f;
	static constexpr float DELETING_PIVOT_MAX_FLOAT = 12.f;
	static constexpr float PLAYER_DIE_FLOAT = 17.f;


public:
	Room(uint64 roomId, UINT8 pivotsSize);
	virtual ~Room();

public:
	void Enter(ClientSessionRef client);
	virtual void OnExitGame(ClientSessionRef client);
	void BackToLobby(ClientSessionRef	clientSession);

	void OnEnterRoomFinished(ClientSessionRef client, BYTE* buffer, int32 len);


protected:
	void LoadLevel(ClientSessionRef welcomedClient);

public:
	uint64 PlayersCount() { return _clients.size(); }
	UINT64 RoomId() { return _roomId; }

protected:
	uint64 _roomId;
	Set<ClientSessionRef> _clients;
	Map<UINT64, PlayerRef> _players;

/*----------------------
	  Room Utils
----------------------*/

protected:
	void InformRoomInfos();

protected:
	void CastAll(SendBufferRef sendBuffer);
	void CastOthers(ClientSessionRef exceptClient, SendBufferRef sendBuffer);
	void CastOnly(ClientSessionRef onlyClient, SendBufferRef sendBuffer);

/*-------------------
	 HeightObject
-------------------*/
public:
	void CreateHeightObject(float currentHeight, UINT32 score);
	void DeleteHeightObject(float currentHeight);

protected:
	void CreateEnergyObject(UINT32 score);

protected:
	Vector<HeightObjectRef> _heightObjects;
	UINT8 _heightObjectsSize;
	UINT8 _maxHeightIndex = 0;
	UINT8 _leastHeightIndex;



/*---------------
	  Pivot
---------------*/
protected:
	PivotRef CreatePivot_PrePos(UINT32 score, Vector2 prePos);
	Vector2 CreateVector2_PrePos(UINT32 score, Vector2 prePos);
	PivotRef CretaePivot(UINT32 score, Vector2 instanPos, UINT64 ownerId, UINT8 index);

protected:
	Vector2 _playersHighestPos;
	Vector2 _playersLowestPos;

protected:
	std::mt19937 _rng;

	std::uniform_int_distribution<int> hundredDist;

	struct DistSet
	{
		std::uniform_int_distribution<int> distX;
		std::uniform_int_distribution<int> distY;
	};

	Vector<DistSet> _pivotDistSets;

public:
	void DeleteLoosePivotAfterTime(UINT8 index);
	void AddressLoosePivot_AfterMergeFinished(UINT8 index);

/*----------------
      Energy
----------------*/
public:
	void Handle_REQUIRE_ENERGY(ClientSessionRef client, BYTE* buffer, int32 len);

protected:
	UINT8 _energyQuota;
	std::uniform_int_distribution<int> _energyDist;

/*----------------
	   Bird
----------------*/
public:
	void UpdateBirdPos(UINT8 index, UINT16 objectCode);


/*---------------
	  Player
---------------*/
public:
	virtual void UpdatePlayerRotPos(ClientSessionRef client, BYTE* buffer, int32 len);
	virtual void Handle_ANNOUNCE_JUMP(ClientSessionRef client, BYTE* buffer, int32 len);
	virtual void Handle_REQUIRE_ATTACH_TO_PIVOT(ClientSessionRef client, BYTE* buffer, int32 len);
	virtual void Handle_PLAYER_DIE_TRIGGERED(ClientSessionRef client, BYTE* buffer, int32 len);
	virtual void Handle_PLAYER_FLIP(ClientSessionRef client, BYTE* buffer, int32 len);
	void Handle_REQUIRE_BLUE_SELF_RACKET(ClientSessionRef client, BYTE* buffer, int32 len);
	void Handle_SYNC_PLAYER_ANIMATION(ClientSessionRef client, BYTE* buffer, int32 len);
	void Handle_BLUE_SIEZE_MODE(ClientSessionRef client, BYTE* buffer, int32 len);
	void Handle_CANCEL_BLUE_SIEZE_MODE(ClientSessionRef client, BYTE* buffer, int32 len);
	void Handle_SIEGE_MODE_FIRE(ClientSessionRef client, BYTE* buffer, int32 len);
	void SIEGE_MODE_FIRE(UINT64 siegingPlayerId, UINT64 flyingPlayerId);
	void Handle_ANNOUNCE_SWING(ClientSessionRef client, BYTE* buffer, int32 len);
	void Handle_ANNOUNCE_IDLE(ClientSessionRef client, BYTE* buffer, int32 len);
};