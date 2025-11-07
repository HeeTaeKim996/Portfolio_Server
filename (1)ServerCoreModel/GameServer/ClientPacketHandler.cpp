#include "pch.h"
#include "ClientPacketHandler.h"

#include "ClientSession.h"

#include "FlatBufferReader.h"
#include "FlatBufferWriter.h"

#include "Room.h"
#include "CapsuleRoom.h"
#include "MultiRoom.h"
#include "Lobby.h"
#include "Player.h"

ClientHandlerFunc GClientHandler[UINT16_MAX];





bool ClientPacketHandler::Handle_INVALID(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	
	// TODO : log
#ifdef _DEBUG
	cout << header->id << endl;

	CRASH("HANDLE_INVALID");
#endif 



	return false;
}

bool ClientPacketHandler::Handle_CREATE_ID(const ClientSessionRef& session, BYTE* buffer, int32 len)
{	
	wstring login_id = DBFuncs::GenerateID();



	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_CREATE_ID);
		fbw.WriteWString(login_id);
	}

	session->Send(sendBuffer);

	return true;
}

bool ClientPacketHandler::Handle_ENTER_GAME(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);

	{
		wstring loginId = fbr.ReadWString();

		string st = WStringCoverter::WStringToString(loginId);

		sql::SQLString sqlSt(st.c_str());



		DB db = DB::Pop();
		db.Pstmt() = db.Con()->prepareStatement
		("SELECT id, best_score, nick_name FROM user_infos WHERE login_id = ?;");

		db.Pstmt()->setString(1, sqlSt);

		db.Res() = db.Pstmt()->executeQuery();

		UINT64 id;
		UINT32 bestScore;


		wstring nick_name;
		
		if(db.Res()->next())
		{
			id = db.Res()->getUInt64("id");
			bestScore = db.Res()->getUInt("best_score");
			sql::SQLString sqlSt = db.Res()->getString("nick_name");
			string st = sqlSt.c_str();
			nick_name = WStringCoverter::StringToWString(st);
			

			session->SetPlayerId(id);
			session->SetBestScore(bestScore);
			session->SetNickName(nick_name);
			
			GLobby->DoAsync(&Lobby::AcceptClient, session);


			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, S_ENTER_GAME);
				fbw.Write<UINT64>(id);
				fbw.Write<UINT32>(bestScore);
				fbw.WriteWString(nick_name);
			}
			session->Send(sendBuffer);
		}
		else
		{
			SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
			{
				FlatBufferWriter fbw(sendBuffer, S_LOGIN_FAILED);
			}

			session->Send(sendBuffer);
		}
	}

	return true;
}

bool ClientPacketHandler::Handle_MAKE_ROOM(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	GLobby->DoAsync(&Lobby::MakeCapsuleRoom, session);

	return true;
}

bool ClientPacketHandler::Handle_ENTER_ROOM_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (const RoomRef& room = session->Room())
	{
		room->DoAsync(&Room::OnEnterRoomFinished, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_JOIN_ROOM(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);

	uint64 roomId = fbr.Read<uint64>();


	// TODO

	return true;
}

bool ClientPacketHandler::Handle_BACK_TO_LOBBY(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->BackToLobby(session);
	}
	
	return true;
}


bool ClientPacketHandler::Handle_KEPP_ALIVE(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}

bool ClientPacketHandler::Handle_PLAYER_UPDATE_ROT_POS(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (const RoomRef& room = session->Room())
	{
		room->DoAsync(&Room::UpdatePlayerRotPos, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_REQUIRE_ATTACH_TO_PIVOT(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (const RoomRef& room = session->Room())
	{
		room->DoAsync(&Room::Handle_REQUIRE_ATTACH_TO_PIVOT, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_ANNOUNCE_JUMP(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (const RoomRef& room = session->Room())
	{
		room->DoAsync(&Room::Handle_ANNOUNCE_JUMP, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_PLAYER_DIE_TRIGGERED(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (const RoomRef& room = session->Room())
	{
		room->DoAsync(&Room::Handle_PLAYER_DIE_TRIGGERED, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_ENTER_LOBBY_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	GLobby->DoAsync(&Lobby::InformLobbyInfos);

	return true;
}

bool ClientPacketHandler::Handle_MERGE_ROOM_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		CapsuleRoomRef capsuleRoom = static_pointer_cast<CapsuleRoom>(room);
		if (MultiRoomRef multiRoom = capsuleRoom->MultiRoom())
		{
			multiRoom->DoAsync(&MultiRoom::Multi_OnMergeRoomFinished, session, buffer, len);
		}
	}

	return true;
}

bool ClientPacketHandler::Handle_SPLIT_ROOM_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		MultiRoomRef multiRoom = static_pointer_cast<MultiRoom>(room);
		if (const CapsuleRoomRef& capsuleRoom = multiRoom->GetCapsuleRoom(session->PlayerId()))
		{
			capsuleRoom->DoAsync(&CapsuleRoom::Handle_C_SPLIT_ROOM_FINISHED, session, buffer, len);
		}
	}

	return true;
}

bool ClientPacketHandler::Handle_UPDATE_NICKNAME(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);
	wstring nickName = fbr.ReadWString();

	if (nickName.size() <= 0)
	{
		cout << "ClientPAcketHandler.cpp__DEBUG : " << endl;

		return false;
	}

	string st = WStringCoverter::WStringToString(nickName);
	sql::SQLString sqlSt(st.c_str());

	{
		DB db = DB::Pop();
		db.Pstmt() = db.Con()->prepareStatement
		("UPDATE user_infos SET nick_name = ? WHERE id = ?;");

		db.Pstmt()->setString(1, sqlSt);
		db.Pstmt()->setUInt64(2, session->PlayerId());

		db.Pstmt()->executeUpdate();
	}

	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, S_UPDATE_NICK_NAME);
		fbw.WriteWString(nickName);
	}
	session->Send(sendBuffer);
	

	return true;
}

bool ClientPacketHandler::Handle_PLAYER_FLIP(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_PLAYER_FLIP, session, buffer, len);
	}


	return true;
}

bool ClientPacketHandler::Handle_REQUIRE_ENERGY(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_REQUIRE_ENERGY, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_REQUIRE_BLUE_SELF_RACKET(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_REQUIRE_BLUE_SELF_RACKET, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_SYNC_PLAYER_ANIMATION(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_SYNC_PLAYER_ANIMATION, session, buffer, len);
	}


	return true;
}

bool ClientPacketHandler::Handle_BLUE_SIEZE_MODE(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_BLUE_SIEZE_MODE, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_CANCEL_BLUE_SIEZE_MODE(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_CANCEL_BLUE_SIEZE_MODE, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_SIEGE_MODE_FIRE(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_SIEGE_MODE_FIRE, session, buffer, len);
	}


	return true;
}

bool ClientPacketHandler::Handle_ANNOUNCE_SWING(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_ANNOUNCE_SWING, session, buffer, len);
	}

	return true;
}

bool ClientPacketHandler::Handle_ANNOUNCE_IDLE(const ClientSessionRef& session, BYTE* buffer, int32 len)
{
	if (RoomRef room = session->Room())
	{
		room->DoAsync(&Room::Handle_ANNOUNCE_IDLE, session, buffer, len);
	}

	return true;
}




