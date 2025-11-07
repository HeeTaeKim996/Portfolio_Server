#include "pch.h"
#include "StrongBird.h"
#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 
#include "Room.h"


StrongBird::StrongBird(UINT64 ownerId, UINT16 index, Vector2 pos, float rot, float _maxMoveDistance,
	float movementSpeed)
	: Pivot(ownerId, index, pos, rot, O_STRONG_BIRD), _centerPos(pos), _isMovingRight(true),
	_recentMoveTick(GetTickCount64()), _maxMoveDistance(_maxMoveDistance), 
	_movementSpeed(movementSpeed), beforeRoomMoveOwnerId(ownerId)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddStrongBird();
#endif
}

StrongBird::~StrongBird()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveStrongBird();
#endif 

}

HeightObjectRef StrongBird::Clone() const
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddStrongBird();
#endif 

	return ObjectPool<StrongBird>::MakeShared_DeepCopy(*this);
}

Vector2 StrongBird::UpdatePos()
{
	UINT64 currentTick = GetTickCount64();
	UINT64 moveTick = currentTick - _recentMoveTick;
	_recentMoveTick = currentTick;
	float moveDistance = moveTick * _movementSpeed;

	float x;
	if (_isMovingRight)
	{
		x = _pos.x + moveDistance;
		if (x > _centerPos.x + _maxMoveDistance)
		{
			x = _centerPos.x + _maxMoveDistance;
			_isMovingRight = false;
		}
	}
	else
	{
		x = _pos.x - moveDistance;
		if (x < _centerPos.x - _maxMoveDistance)
		{
			x = _centerPos.x - _maxMoveDistance;
			_isMovingRight = true;
		}
	}

	_pos = Vector2(x, _pos.y);

	return _pos;
}
