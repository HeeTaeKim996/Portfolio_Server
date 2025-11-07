using System;
using System.Collections.Specialized;
using CSharpServerCore;

namespace CSharpSampleServer
{
    internal class ServerSession : Session
    {
        public CUserToken token { get; private set; }
        public ServerSession(CUserToken token)
        {
            this.token = token;
            this.token.SetSession(this);

            { // 연결 성공후 ServerSesison 생성시, 서버에 ENTER_GAME 패킷 전송

                CPacket sendBuffer = CPacket.PopForCreate((UInt16)C.C_ENTER_GAME);
                Send(sendBuffer);
            }
        }

        public override void OnMessage(ConstBuffer buffer)
        {
            CPacket msg = CPacket.PopForRead(buffer);

            S s = (S)msg.Pop_UInt16();
            switch (s)
            {
                case S.S_INFORM_LOBBY_INFOS:
                    {
                        UInt32 roomCount = msg.Pop_UInt32();
                        Console.WriteLine($"RoomCount : {roomCount}");
                        Console.WriteLine("--------------------------------");
                        for(int i = 0; i < roomCount; i++)
                        {
                            Console.WriteLine($"Room ID : {msg.Pop_UInt64()}");
                        }
                    }
                    break;

                case S.S_INFORM_ROOM_INFOS:
                    {
                        Console.WriteLine($"Players In Room : {msg.Pop_UInt32()}");
                    }
                    break;
                case S.S_TEST_CHAT:
                    {
                        Console.WriteLine($"Recv Wstr : {msg.Pop_String()}");
                    }
                    break;
            }


            CPacket.PushBack(msg);
        }


        public override void Send(CPacket msg)
        {
            token.Send(msg);
            CPacket.PushBack(msg);
        }
        public override void OnRemoved()
        {
            
        }
        public override void Disconnect()
        {
            token.socket.Disconnect(false);
        }

    }
}
