using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using CSharpServerCore;
using UnityEngine;

namespace Network
{
    public class CNetUnityService : MonoBehaviour
    {
        private NetworkManager networkManager;
        private CNetworkService cNetworkService = null;
        private NetEventQueuer netEventQueuer = new NetEventQueuer();
        private ServerSession serverSession;

#if __CPACKET_POOL_COUNT_CHECK
    // 버그인지, CPacketBufferManager의 poolCountChecker 가 참조되지 않음.. 추후 수정 필요    

#endif
    

        private void Awake()
        {
            CPacketBufferManager.Initialize(10);
            networkManager = GetComponent<NetworkManager>();
        }

        public void Connect(string host, int port)
        {
            if(cNetworkService != null)
            {
                Debug.Log("이미 서버와 연결됨");
                return;
            }

            cNetworkService = new CNetworkService();
            CConnector cConnector = new CConnector(cNetworkService);
            cConnector.callback_OnConnected += OnConnected_ToServer;
            IPEndPoint remoteEndPoint = new IPEndPoint(IPAddress.Parse(host), port);
            cConnector.Connect(remoteEndPoint);
        }
        private void OnConnected_ToServer(CUserToken token)
        {
            serverSession = new ServerSession(token);
            serverSession.SetNetEventQueuer(netEventQueuer);
            netEventQueuer.EnqueueNetEvent(NetworkEvent.Connected);
        }

        private void Update()
        {
            while (netEventQueuer.HasNetEvent())
            {
                networkManager.OnNetStatusChanged(netEventQueuer.DequeueNetEvent());
            }
            while (netEventQueuer.HasNetMessage())
            {
                CPacket msg = netEventQueuer.DequeNetMessage();
                networkManager.OnMessage(msg);
                CPacket.PushBack(msg);
            }
        }
        public void Send(CPacket msg)
        {
            try
            {
                serverSession.Send(msg);
                CPacket.PushBack(msg);
            }
            catch(Exception e)
            {
                Debug.Log($"CNetUnityService : {e.Message}");
            }
        }
        public bool IsConnected()
        {
            return cNetworkService != null;
        }
        private void OnApplicationQuit()
        {
            serverSession.token.Disconnect();
        }
    }
}

