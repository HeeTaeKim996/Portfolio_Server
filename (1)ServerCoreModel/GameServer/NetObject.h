#pragma once
class NetObject : enable_shared_from_this<NetObject>
{
public:
	NetObject(UINT64 ownerId, Vector2 pos, float rot, UINT16 objectCode);
	virtual ~NetObject() {}

public:
	void SetPos(Vector2 posVec){ _pos = posVec; }
	Vector2 GetPos() { return _pos; }

	void SetRot(float rot) { _rot = rot; }
	float GetRot() { return _rot; }

	UINT16 ObjectCode() { return _objectCode; }
	UINT64 OwnerId() { return _ownerId; }

protected:
	UINT64 _ownerId;

	Vector2 _pos;

	float _rot;

	UINT16 _objectCode;
};

