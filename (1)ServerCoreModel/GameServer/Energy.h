#pragma once

#include "HeightObject.h"

class Energy : public HeightObject
{
public:
	Energy(UINT64 ownerId, Vector2 pos, float rot, UINT16 objectCode, UINT8 index, UINT8 amount)
		: HeightObject(ownerId, pos, rot, objectCode, index), _amount(amount)
	{
		_category = HeightObject::CategoryType::Energy;
	}
	virtual ~Energy() override {};

	UINT8 Amount() { return _amount; }

	inline virtual HeightObjectRef Clone() const override
	{
		return ObjectPool<Energy>::MakeShared_DeepCopy(*this);
	}


public:
	enum EnergyType : UINT8
	{ 
		Null = 0,
		BlueEnergy = 1,
		RedEnergy = 2,
	};
	EnergyType GetEnergyType() { return _energyType; }

protected:
	UINT8 _amount;
	EnergyType _energyType = EnergyType::Null;

};

