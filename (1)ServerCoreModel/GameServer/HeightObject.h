#pragma once
#include "NetObject.h"

class HeightObject : public NetObject
{
public:
	HeightObject(UINT64 ownerId, Vector2 pos, float rot, UINT16 objectCode, UINT8 index)
		: NetObject(ownerId, pos, rot, objectCode), _index(index) {}
	virtual ~HeightObject() override {}

	UINT8 Index() { return _index; }
	void SwitchIndex(UINT8 newIndex) { _index = newIndex; }
	void SwitchOwnerId(UINT8 newOwnerId) { _ownerId = newOwnerId; }

	virtual HeightObjectRef Clone() const = 0;

	void MoveToward(const Vector2& delta) { _pos = _pos + delta; }


public:
	enum CategoryType
	{
		Pivot,
		Energy
	};
	HeightObject::CategoryType Category() { return _category; }


protected:
	UINT8 _index;
	CategoryType _category;
};

