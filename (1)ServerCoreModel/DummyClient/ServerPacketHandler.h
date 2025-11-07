#pragma once

#include <functional>

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];



class ServerPacketHandler
{

public:
	inline static void Init()
	{
		for (int i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID;
		}

		GPacketHandler[S_ENTER_GAME] = Handle_ENTER_GAME;
		GPacketHandler[S_INFORM_LOBBY_INFOS] = Handle_INFORM_LOBBY_INFOS;
		GPacketHandler[S_INFORM_ROOM_INFOS] = Handle_INFORM_ROOM_INFOS;
		GPacketHandler[S_TEST_CHAT] = Handle_TEST_CHAT;
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}


private:
	static bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);


	static bool Handle_ENTER_GAME(PacketSessionRef& session, BYTE* buffer, int32 len);

	static bool Handle_INFORM_LOBBY_INFOS(PacketSessionRef& session, BYTE* buffer, int32 len);

	static bool Handle_INFORM_ROOM_INFOS(PacketSessionRef& session, BYTE* buffer, int32 len);

	static bool Handle_TEST_CHAT(PacketSessionRef& session, BYTE* buffer, int32 len);
};

