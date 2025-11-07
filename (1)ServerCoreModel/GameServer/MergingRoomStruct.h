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
		/*   ※ Player, Pivot의 RefCount 보장
			- shared_ptr을 복사했기 때문에, MultiRoom에서 _pivots를 하나로 병합할 때에, CapsuleRoom에서 _pivot 중 하나를
			  삭제해도, MergingRoomStruct로 인해 refCount > 0 이기 때문에, delete되지 않는다
		*/
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