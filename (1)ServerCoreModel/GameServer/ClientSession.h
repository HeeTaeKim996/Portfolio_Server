#pragma once

#include "Session.h"

class Player;

class ClientSession : public PacketSession
{
	friend class ClientPacketHandler;

public:
	ClientSession();
	virtual ~ClientSession();

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:

	void SetPlayerId(uint64 playerId) { _playerId = playerId; }
	uint64 PlayerId() { return _playerId; }
	void SetBestScore(UINT32 bestScore) { _bestScore = bestScore; }
	void UpdateScore(UINT32 score);
	void SetNickName(wstring nickName) { _nickName = nickName; }

	UINT32 BestScore() { return _bestScore; }
	wstring NickName() { return _nickName; }

	inline void EnterRoom(RoomRef room) 
	{
		_room = room;
	}
	inline void SwitchRoom(RoomRef room)
	{
		_room = room;
	}
	const RoomRef& Room() { return _room.lock(); }

	void OnExitRoom();

	void SetPlayerObject(shared_ptr<Player> player) { _playerObject = player; }
	const PlayerRef& PlayerObject() { return _playerObject.lock(); }

private:
	uint64 _playerId = 0;
	UINT32 _bestScore = 0;
	wstring _nickName;

	weak_ptr<class Room> _room;

	weak_ptr<Player> _playerObject;
};
