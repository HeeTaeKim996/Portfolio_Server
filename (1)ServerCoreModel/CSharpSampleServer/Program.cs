using System;
using System.Collections.Generic;
using System.Net;
using CSharpServerCore;

namespace CSharpSampleServer
{
    class Program
    {
        private static List<ServerSession> _serverSessions = new List<ServerSession>();
        private static object lock_serverSessions = new object();


        static void Main(string[] args)
        {
            CPacketBufferManager.Initialize(10);

            CNetworkService cNetworkService = new CNetworkService();

            CConnector cConnector = new CConnector(cNetworkService);
            cConnector.callback_OnConnected += OnConnected_ToServer;
            IPEndPoint remoteEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7777);
            cConnector.Connect(remoteEndPoint);

            Thread inputThread = new Thread(RecvConsoleInput);
            inputThread.Start();



        }

        private static void OnConnected_ToServer(CUserToken token)
        {
            ServerSession serverSession = new ServerSession(token);
            lock (lock_serverSessions)
            {
                _serverSessions.Add(serverSession);
            }

            Console.WriteLine("서버와 연결 성공");
        }

        private static void RecvConsoleInput()
        {
            Thread blabberThread = new Thread(DoBlabber);

            while (true)
            {
            Lobby:
                Console.WriteLine("Make Room : 0 / Join Room : 1");
                string input = Console.ReadLine();
                if(input == 0.ToString())
                {
                    CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_MAKE_ROOM);
                    _serverSessions[0].Send(sendPacket);

                    goto Room;
                }
                else if(input == 1.ToString())
                {
                    Console.Write("Entering Room Id : ");
                    UInt64 roomId;

                    string stInput = Console.ReadLine();
                    if(!UInt64.TryParse(stInput, out roomId))
                    {
                        Console.WriteLine("불가능 id");
                    }
                    else
                    {
                        CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_JOIN_ROOM);
                        sendPacket.Push((UInt64)roomId);
                        _serverSessions[0].Send(sendPacket);
                        goto Room;
                    }
                }
                Room:
                while (true)
                {
                    string inputStr = Console.ReadLine();

                    if(inputStr == "Exit_Game")
                    {
                        CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_BACK_TO_LOBBY);
                        _serverSessions[0].Send(sendPacket);
                        goto Lobby;
                    }
                    else if (inputStr == "Blabber")
                    {
                        blabberThread.Start();
                    }
                    else
                    {
                        CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_TEST_CHAT);
                        sendPacket.Push(inputStr);
                        _serverSessions[0].Send(sendPacket);
                    }
                }
            }
        }
        private static void DoBlabber()
        {

            while (true)
            {
                CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_TEST_CHAT);
                string str = "Blabber";
                sendPacket.Push(str);

                _serverSessions[0].Send(sendPacket);

                Thread.Sleep(20);
            }
        }
    }


}