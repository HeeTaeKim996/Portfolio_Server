using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace CSharpServerCore
{
    public class CConnector
    {
        private CNetworkService _cNetworkService;
        private Socket clientSocket;

        public delegate void ConnectHandler(CUserToken token);
        public ConnectHandler callback_OnConnected;

        public CConnector(CNetworkService cNetworkService)
        {
            _cNetworkService = cNetworkService;
        }

        public void Connect(IPEndPoint remoteEndPoint)
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            SocketAsyncEventArgs connectArgs = new SocketAsyncEventArgs();
            connectArgs.Completed += OnConnectCompleted;
            connectArgs.RemoteEndPoint = remoteEndPoint;

            bool pending = clientSocket.ConnectAsync(connectArgs);
            if (!pending)
            {
                OnConnectCompleted(null, connectArgs);
            }
        }

        private void OnConnectCompleted(object sender, SocketAsyncEventArgs e)
        {
            if(e.SocketError == SocketError.Success)
            {
                CUserToken token = new CUserToken();
                _cNetworkService.OnCConnector_ConnectCompleted(clientSocket, token);
                callback_OnConnected?.Invoke(token);
            }
        }
    }
}
