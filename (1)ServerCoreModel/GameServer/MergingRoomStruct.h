#pragma once


struct MergingRoomStruct
{
public:
	MergingRoomStruct(ClientSessionRef inputClientSession, const PlayerRef& inputPlayer, 
		Vector<HeightObjectRef> inputHeightObjects,UINT8 inputMaxHeightIndex, UINT8 inputLeastHeightIndex, UINT64 inputOwnerId)
		: clientSession(inputClientSession), player(inputPlayer), heightObjects(inputHeightObjects),
		maxHeightIndex(inputMaxHeightIndex), leastHeightIndex(inputLeastHeightIndex), 
		ownerId(inputOwnerId)
	{
	}

	ClientSessionRef clientSession;
	PlayerRef player;
	Vector<HeightObjectRef> heightObjects;
	UINT8 maxHeightIndex;
	UINT8 leastHeightIndex;
	UINT64 ownerId;
};

struct RetrievingRoomStruct
{
public:
	RetrievingRoomStruct(Vector<HeightObjectRef>& inputHeightObjects, UINT8 inputMaxHeightIndex)
		: heightObjects(inputHeightObjects), maxHeightIndex(inputMaxHeightIndex)
	{
	}

	Vector<HeightObjectRef>& heightObjects;
	UINT8 maxHeightIndex;
};