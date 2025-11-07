#pragma once

#include "pch.h"
#include "NetObject.h"

class Player : public NetObject
{
public:
	Player(uint64 ownerId, Vector2 pos, float rot, UINT16 objectCode, ClientSessionRef client);
	virtual ~Player();
	void SetPivotIndex(UINT8 pivotIndex) { _pivotIndex = pivotIndex; }



	ClientSessionRef Client() { return _client.lock(); }

	UINT8 PivotIndex() { return _pivotIndex; }

	void Update_HeightAndInterpolation(float interpolation);
	float BestHeight() { return _bestHeight; }

	UINT32 Score() { return _score; }
	void UpdateScore(float updateBestHeight);

	UINT8 AddBlueEnergy(UINT8 plusEnergy);
	UINT8 RemoveBlueEnergy(UINT8 removeEnergy);

	UINT8 RedEnergy() { return _redEnergy; }
	UINT8 BlueEnergy() { return _blueEnergy; }

	void SwitchPlayerState(UINT8 newState) { _playerState = newState; }
	UINT8 PlayerState() { return _playerState; }

private:
	weak_ptr<ClientSession> _client;
	UINT8 _pivotIndex;
	UINT32 _score = 0;
	UINT8 _redEnergy = 0;
	UINT8 _blueEnergy = 100;
	UINT8 _playerState = PS_Jumping;


	float _bestHeight = 0;
	float _heightInterpolation = 0;
};

