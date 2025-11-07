#pragma once

#include "Pivot.h"

class LoosePivot : public Pivot
{
public:
	LoosePivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot);
	virtual ~LoosePivot() override;

	virtual HeightObjectRef Clone() const override;

	void MakeWobble(UINT64 afterDeleteTick);
	bool IsWobble() { return _isWobble; }
	UINT64 DeleteTickTime() { return _deleteTickTime; }

private:
	bool _isWobble = false;
	UINT64 _deleteTickTime;
};

