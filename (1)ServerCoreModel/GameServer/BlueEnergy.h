#pragma once
#include "Energy.h"

class BlueEnergy : public Energy
{
public:
	BlueEnergy(UINT64 ownerId, Vector2 pos, float rot, UINT16 objectCode, UINT8 index,
		UINT8 amount);
	virtual ~BlueEnergy() override;

	virtual HeightObjectRef Clone() const override;
};

