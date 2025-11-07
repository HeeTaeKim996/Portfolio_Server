using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace CSharpServerCore
{
    public class CNetworkService
    {

        public void OnCConnector_ConnectCompleted(Socket socket, CUserToken token)
        {
            SocketAsyncEventArgs recvArgs = new SocketAsyncEventArgs();
            recvArgs.Completed += OnRecvCompleted;
            recvArgs.SetBuffer(new byte[StaticValues.bufferSize], 0, StaticValues.bufferSize);
            recvArgs.UserToken = token;

            SocketAsyncEventArgs sendArgs = new SocketAsyncEventArgs();
            sendArgs.Completed += OnSendCompleted;
            sendArgs.SetBuffer(new byte[StaticValues.bufferSize], 0, StaticValues.bufferSize);
            sendArgs.UserToken = token;

            StartRecv(socket, recvArgs, sendArgs);
        }

        private void OnRecvCompleted(object sender, SocketAsyncEventArgs e)
        {
            if(e.LastOperation == SocketAsyncOperation.Receive)
            {
                ProcessRecv(e);
            }
        }

        private void OnSendCompleted(object sender, SocketAsyncEventArgs e)
        {
            CUserToken token = e.UserToken as CUserToken;
            token.ProcessSend(e);
        }



        private void StartRecv(Socket socket, SocketAsyncEventArgs recvArgs, SocketAsyncEventArgs sendArgs)
        {
            CUserToken token = recvArgs.UserToken as CUserToken;
            token.socket = socket;
            token.SetArgs(recvArgs, sendArgs);

            bool pending = socket.ReceiveAsync(recvArgs);
            if (!pending)
            {
                ProcessRecv(recvArgs);
            }
        }

        private void ProcessRecv(SocketAsyncEventArgs e)
        {
            CUserToken token = e.UserToken as CUserToken;

            if(e.BytesTransferred > 0 && e.SocketError == SocketError.Success)
            {
                token.OnReceiveTcp(e.Buffer, e.Offset, e.BytesTransferred);

                bool pending = token.socket.ReceiveAsync(token.recvArgs);
                if (!pending)
                {
                    OnRecvCompleted(null, token.recvArgs);
                }
            }
            else
            {
                OnServerShutDown(token);
            }
        }

        private void OnServerShutDown(CUserToken token)
        {
            token.OnRemoved();

            // TODO : 뭐하지?
        }
    }
}
