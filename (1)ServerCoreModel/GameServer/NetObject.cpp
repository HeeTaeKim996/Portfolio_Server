#include "pch.h"
#include "NetObject.h"

NetObject::NetObject(UINT64 ownerId, Vector2 pos, float rot, UINT16 objectCode)
	: _ownerId(ownerId), _pos(pos), _rot(rot), _objectCode(objectCode)
{
}
