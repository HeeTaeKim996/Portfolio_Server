#include "pch.h"
#include "ServerSession.h"

#include "FlatBufferWriter.h"
#include "FlatBufferReader.h"
#include "ServerPacketHandler.h"





void ServerSession::OnConnected() 
{
	cout << "Connected To Server" << endl;

	SendBufferRef sendBuffer = GSendBufferManager->MakeSendBuff();
	{
		FlatBufferWriter fbw(sendBuffer, C_ENTER_GAME);
	}
	Send(sendBuffer);
}


void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	ServerPacketHandler::HandlePacket(session, buffer, len);
}