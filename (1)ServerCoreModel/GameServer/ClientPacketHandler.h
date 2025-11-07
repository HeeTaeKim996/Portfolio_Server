#pragma once

#include <functional>

using ClientHandlerFunc = std::function<bool(const ClientSessionRef&, BYTE*, int32)>;
extern ClientHandlerFunc GClientHandler[UINT16_MAX];





class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			GClientHandler[i] = Handle_INVALID;
		}

		GClientHandler[C_ENTER_GAME] = Handle_ENTER_GAME;
		GClientHandler[C_CREATE_ID] = Handle_CREATE_ID;
		GClientHandler[C_MAKE_ROOM] = Handle_MAKE_ROOM;
		GClientHandler[C_JOIN_ROOM] = Handle_JOIN_ROOM;
		GClientHandler[C_BACK_TO_LOBBY] = Handle_BACK_TO_LOBBY;
		GClientHandler[C_KEEP_ALIVE] = Handle_KEPP_ALIVE;
		GClientHandler[C_PLAYER_UPDATE_ROT_POS] = Handle_PLAYER_UPDATE_ROT_POS;
		GClientHandler[C_REQUIRE_ATTACH_TO_PIVOT] = Handle_REQUIRE_ATTACH_TO_PIVOT;
		GClientHandler[C_ANNOUCE_JUMP] = Handle_ANNOUNCE_JUMP;
		GClientHandler[C_ENTER_ROOM_FINISHED] = Handle_ENTER_ROOM_FINISHED;
		GClientHandler[C_PLAYER_DIE_TRIGGERED] = Handle_PLAYER_DIE_TRIGGERED;
		GClientHandler[C_ENTER_LOBBY_FINISHED] = Handle_ENTER_LOBBY_FINISHED;
		GClientHandler[C_MERGE_ROOM_FINISHED] = Handle_MERGE_ROOM_FINISHED;
		GClientHandler[C_SPLIT_ROOM_FINISHED] = Handle_SPLIT_ROOM_FINISHED;
		GClientHandler[C_UPDATE_NICK_NAME] = Handle_UPDATE_NICKNAME;
		GClientHandler[C_PLAYER_FLIP] = Handle_PLAYER_FLIP;
		GClientHandler[C_REQUIRE_ENERGY] = Handle_REQUIRE_ENERGY;
		GClientHandler[C_REQUIRE_BLUE_SELF_RACKET] = Handle_REQUIRE_BLUE_SELF_RACKET;
		GClientHandler[C_SYNC_PLAYER_ANIMATION] = Handle_SYNC_PLAYER_ANIMATION;
		GClientHandler[C_BLUE_SIEZE_MODE] = Handle_BLUE_SIEZE_MODE;
		GClientHandler[C_REQUIRE_CANCEL_SIEZE_MODE] = Handle_CANCEL_BLUE_SIEZE_MODE;
		GClientHandler[C_BLUE_SIZE_MODE_FIRE] = Handle_SIEGE_MODE_FIRE;
		GClientHandler[C_ANNOUNCE_SWING] = Handle_ANNOUNCE_SWING;
		GClientHandler[C_ANNOUNCE_IDLE] = Handle_ANNOUNCE_IDLE;
	}

	inline static bool HandlePacket(const ClientSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GClientHandler[header->id](session, buffer, len);
	}



private:
	static bool Handle_INVALID(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_CREATE_ID(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_ENTER_GAME(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_MAKE_ROOM(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_ENTER_ROOM_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_JOIN_ROOM(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_BACK_TO_LOBBY(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_KEPP_ALIVE(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_PLAYER_UPDATE_ROT_POS(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_REQUIRE_ATTACH_TO_PIVOT(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_ANNOUNCE_JUMP(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_PLAYER_DIE_TRIGGERED(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_ENTER_LOBBY_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_MERGE_ROOM_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_SPLIT_ROOM_FINISHED(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_UPDATE_NICKNAME(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_PLAYER_FLIP(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_REQUIRE_ENERGY(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_REQUIRE_BLUE_SELF_RACKET(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_SYNC_PLAYER_ANIMATION(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_BLUE_SIEZE_MODE(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_CANCEL_BLUE_SIEZE_MODE(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_SIEGE_MODE_FIRE(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_ANNOUNCE_SWING(const ClientSessionRef& session, BYTE* buffer, int32 len);
	static bool Handle_ANNOUNCE_IDLE(const ClientSessionRef& session, BYTE* buffer, int32 len);
};	

