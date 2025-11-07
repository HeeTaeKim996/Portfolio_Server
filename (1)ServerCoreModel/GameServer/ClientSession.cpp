#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

#include "ClientPacketHandler.h"
#include "Room.h"
#include "Lobby.h"

#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 


ClientSession::ClientSession()
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddClientSession();
#endif
}

ClientSession::~ClientSession()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveClientSession();
#endif 

}

void ClientSession::OnConnected()
{
	GSessionManager->Add(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	GSessionManager->Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	ClientPacketHandler::HandlePacket(static_pointer_cast<ClientSession>(shared_from_this()), buffer, len);
}

void ClientSession::OnSend(int32 len)
{
	{ // DEBUG
		//cout << "OnSend Len = " << len << endl;
	}
}

void ClientSession::UpdateScore(UINT32 score)
{
	if (score > _bestScore)
	{
		_bestScore = score;

		cout << "ClientSession.cpp__DEBUG : " << score << endl;

		

		DB db = DB::Pop();
		db.Pstmt() = db.Con()->prepareStatement
		("UPDATE user_infos SET best_score = ? WHERE id = ?;");

		db.Pstmt()->setUInt(1, score);
		db.Pstmt()->setUInt64(2, _playerId);

		db.Pstmt()->executeUpdate();
	}


}



void ClientSession::OnExitRoom()
{
	_room.reset();
}