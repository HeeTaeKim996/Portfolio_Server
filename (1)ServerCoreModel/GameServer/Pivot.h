#pragma once

#include "HeightObject.h"

class Pivot : public HeightObject
{
public:
	Pivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot, UINT16 objectCode) 
		: HeightObject(ownerId, pos, rot, objectCode, index)
	{
		_category = HeightObject::CategoryType::Pivot;
	}
	virtual ~Pivot() override {}

	inline virtual HeightObjectRef Clone() const override
	{
		return ObjectPool<Pivot>::MakeShared_DeepCopy(*this);
	}
};

