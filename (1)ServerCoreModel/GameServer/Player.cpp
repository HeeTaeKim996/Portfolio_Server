#include "pch.h"
#include "Player.h"
#include "FlatBufferWriter.h"
#include "FlatBufferReader.h"
#include "Room.h"
#include "ClientSession.h"
#include "MemoryCount.h"

Player::Player(uint64 ownerId, Vector2 pos, float rot, UINT16 objectCode, ClientSessionRef client)
	:NetObject(ownerId, pos, rot, objectCode)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddPlayer();
#endif // DEBUG

	_client = weak_ptr<ClientSession>(client);

}

Player::~Player()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemovePlayer();
#endif 

}

void Player::Update_HeightAndInterpolation(float interpolation)
{
	_bestHeight += interpolation;
	_heightInterpolation -= interpolation;
}

void Player::UpdateScore(float updateBestHeight)
{
	_bestHeight = updateBestHeight;
	_score = updateBestHeight + _heightInterpolation;
}

UINT8 Player::AddBlueEnergy(UINT8 plusEnergy)
{
	_blueEnergy += plusEnergy;
	if (_blueEnergy > 100)
	{
		_blueEnergy = 100;
	}

	return _blueEnergy;
}

UINT8 Player::RemoveBlueEnergy(UINT8 removeEnergy)
{
	_blueEnergy -= removeEnergy;
	if (_blueEnergy < 0)
	{
		CRASH("USE_MORE_THAN_HAVE");
	}

	return _blueEnergy;
}

