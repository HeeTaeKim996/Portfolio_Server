#pragma once
#include "Pivot.h"


class BasePivot : public Pivot
{
public:
	BasePivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot);
	virtual ~BasePivot() override;

	virtual HeightObjectRef Clone() const override;
};

