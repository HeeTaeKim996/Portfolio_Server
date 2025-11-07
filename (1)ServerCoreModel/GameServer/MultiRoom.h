#pragma once
#include "Room.h"
#include "MergingRoomStruct.h"



struct MoveTowardLog
{
	UINT64 playerId;
	Vector2 moveTowardVec;
};

struct SmallHeightLog
{
	HeightObjectRef movedHeightObject;
	UINT8 previousIndex;
	UINT64 ownerId;
};
struct HeightLog
{
	UINT8 index;
	SmallHeightLog smallHeightLog;
};

struct RetrieveLog
{
	UINT8 previousIndex;
	UINT8 newIndex;
};


class MultiRoom : public Room
{
public:
	MultiRoom(CapsuleRoomRef firstRoom, CapsuleRoomRef secondRoom);
	virtual ~MultiRoom() override;
	void Multi_OnMergeRoomFinished(ClientSessionRef client, BYTE* buffer, int32 len);
	virtual void OnExitGame(ClientSessionRef client) override;

private:
	void LinkerPattern_A(Vector<HeightLog>& logs, MergingRoomStruct& first, MergingRoomStruct& second, 
		Vector2& firstMoveToward, Vector2& secondMoveToward, UINT32 averageScore);

private:
	void MergePivots(Vector<HeightLog>& logs, MergingRoomStruct& first, MergingRoomStruct& second, Vector2 firstAim,
		Vector2 secondAim, Vector2& firstMoveToward, Vector2& secondMoveToward);
	void CreatePivot_ForLinker(UINT64 ownerId, Vector<HeightLog>& logs, Vector2 instanPos, 
		UINT32 averageScore);

public:
	CapsuleRoomRef GetCapsuleRoom(UINT64 playerId);
	void RemoveCapsuleRoom(UINT64 playerId);

public:
	virtual void Handle_REQUIRE_ATTACH_TO_PIVOT(ClientSessionRef client, BYTE* buffer, int32 len) override;
	void OnCapsuleRoomTransferFinished(ClientSessionRef client);

private:
	void SplitRoom(ClientSessionRef client);
	enum SplitCause
	{
		DEFAULT,
		PLAYER_EXIT,
		DISTANCE,
	};
	SplitCause splitCause = SplitCause::DEFAULT;

	MoveTowardLog firstMoveTowardLog;
	MoveTowardLog secondMoveTowardLog;


private:
	Map<UINT64, weak_ptr<CapsuleRoom>> _capsuleRooms;
};		