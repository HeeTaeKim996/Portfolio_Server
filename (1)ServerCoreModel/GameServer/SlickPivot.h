#pragma once

#include "Pivot.h"

class SlickPivot : public Pivot
{
public:
	SlickPivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot);
	virtual ~SlickPivot() override;

	virtual HeightObjectRef Clone() const override;
};

