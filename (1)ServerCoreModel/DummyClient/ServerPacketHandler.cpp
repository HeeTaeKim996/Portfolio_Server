#include "pch.h"
#include "ServerPacketHandler.h"

#include "FlatBufferReader.h"
#include "FlatBufferWriter.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool ServerPacketHandler::Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	// TODO : log

	CRASH("HANDLE_INVALID");

	return false;
}



bool ServerPacketHandler::Handle_ENTER_GAME(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	// TODO

	return true;
}

bool ServerPacketHandler::Handle_INFORM_LOBBY_INFOS(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);

	uint32 roomCount = fbr.Read<uint32>();

	cout << "Room Count : " << roomCount << endl;

	cout << "----------------------------------------" << endl;
	for (int i = 0; i < roomCount; i++)
	{
		cout << "Room ID : " << fbr.Read<uint64>() << endl;
	}

	cout << "----------------------------------------" << endl;

	return true;
}	

bool ServerPacketHandler::Handle_INFORM_ROOM_INFOS(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);

	cout << "Players In Room : " << fbr.Read<uint32>() << endl;


	return true;
}

bool ServerPacketHandler::Handle_TEST_CHAT(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	FlatBufferReader fbr(buffer, len);
	wcout << "Received Wstr : " << fbr.ReadWString() << endl;

	return true;

}



