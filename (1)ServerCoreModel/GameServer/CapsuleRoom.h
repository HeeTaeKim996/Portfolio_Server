#pragma once

#include "Room.h"
#include "MergingRoomStruct.h"



class CapsuleRoom : public Room
{
public:
	CapsuleRoom(UINT64 roomId);

	virtual ~CapsuleRoom() override;

/*---------------------
	  RoomMatching
---------------------*/
public:
	enum MatchState
	{
		Sleep,
		Matching,
		Matched
	};
	enum : UINT64
	{
		MATCH_START_TICK = 1'000,
		MATCH_CHECK_INTERVAL = 5'000,
		MATCH_SLEEP_TICK = 3'000,
	};


	void MatchingInitialize();
	void StartMatch();
	void MatchCall();
	MatchState GetMatchState() { return _matchState; }
	void OnMatched(MultiRoomRef multiRoom) { _matchState = MatchState::Matched; _multiRoom = multiRoom; }

public:
	void OnRoomTransferFinished();
	void RetrievePlayer(RetrievingRoomStruct retrievingRoomStruct, MultiRoomRef multiRoom);
	void Handle_C_SPLIT_ROOM_FINISHED(ClientSessionRef client, BYTE* buffer, int32 len);
	virtual void OnExitGame(ClientSessionRef client) override;

public:
	MergingRoomStruct MergeSource();
	PlayerRef Player() { return _players[_roomId]; }
	MultiRoomRef MultiRoom() { return _multiRoom.lock(); }

private:
	MatchState _matchState;
	UINT32 _roomScore;
	weak_ptr<class MultiRoom> _multiRoom;
};

