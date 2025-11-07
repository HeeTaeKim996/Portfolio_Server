#pragma once

#include "Pivot.h"

class StrongBird : public Pivot
{
public:
	StrongBird(UINT64 ownerId, UINT16 index, Vector2 pos, float rot, float _maxMoveDistance, 
		float movementSpeed);
	virtual ~StrongBird() override;

	virtual HeightObjectRef Clone() const override;


public:
	Vector2 UpdatePos();
	Vector2 UpdateCenterPos(Vector2 movePos) { _centerPos = _centerPos + movePos; }

private:
	Vector2 _centerPos;
	UINT64 _recentMoveTick;
	float _maxMoveDistance;
	float _movementSpeed;
	bool _isMovingRight;

public:
	UINT64 beforeRoomMoveOwnerId;
};

