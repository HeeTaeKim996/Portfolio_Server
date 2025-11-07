using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace CSharpServerCore
{
    public class CUserToken
    {
        public Socket socket { get; set; }
        private Session _session = null;
        public SocketAsyncEventArgs recvArgs { get; private set; }
        public SocketAsyncEventArgs sendArgs { get; private set; }

        private TcpMsgResolver _tcpMsgResolver = new TcpMsgResolver();

        private Queue<CPacket> sendingQueue = new Queue<CPacket>();
        private object lock_sendingQueue = new object();

        public void SetSession(Session session)
        {
            _session = session;
        }
        public void SetArgs(SocketAsyncEventArgs recvArgs, SocketAsyncEventArgs sendArgs)
        {
            this.recvArgs = recvArgs;
            this.sendArgs = sendArgs;
        }

        public void OnReceiveTcp(byte[] buffer, int offset, int transferred)
        {
            _tcpMsgResolver.OnReceiveTcp(buffer, offset, transferred, OnMessage);
        }
        private void OnMessage(ConstBuffer constBuffer)
        {
            _session.OnMessage(constBuffer);
        }

        public void Send(CPacket msg)
        {
            CPacket copyPacket = CPacket.PopForSend(msg, msg.pos);

            lock (lock_sendingQueue)
            {
                if(sendingQueue.Count <= 0)
                {
                    sendingQueue.Enqueue(copyPacket);

                    StartSend();
                }
                else
                {
                    sendingQueue.Enqueue(copyPacket);
                }
            }
        }
        private void StartSend()
        {
            lock (lock_sendingQueue) 
        // ※ Send - StartSend 에서 데드락이 발생안하는 이유는, C#의 lock 이 동일 스레드 내에서는 재진입이 가능하기 때문
            {
                CPacket peekedPacket = sendingQueue.Peek();
                peekedPacket.RecordSize();

                sendArgs.SetBuffer(0, peekedPacket.pos);
                Buffer.BlockCopy(peekedPacket.buffer, 0, sendArgs.Buffer, 0, peekedPacket.pos);

                bool pending = socket.SendAsync(sendArgs);
                if (!pending)
                {
                    ProcessSend(sendArgs);
                }
            }
        }

        public void ProcessSend(SocketAsyncEventArgs e)
        {
            lock (lock_sendingQueue)
            {
                if (e.BytesTransferred <= 0 || e.SocketError != SocketError.Success) return;

                if (sendingQueue.Peek().pos != e.BytesTransferred)
                {
                    return; // 길이가 다르면, 재전송
                }

                CPacket.PushBack(sendingQueue.Dequeue());

                if(sendingQueue.Count > 0)
                {
                    StartSend();
                }
            }
        }

        public void OnRemoved()
        {
            sendingQueue.Clear();
            if(_session != null)
            {
                _session.OnRemoved();
            }
        }

        public void Disconnect()
        {
            try
            {
                socket.Shutdown(SocketShutdown.Send);
            }
            catch (Exception e) { }

            socket.Close();
        }


        private Timer _keepAliveTimer;

        public void StartKeepAlive()
        {
            _keepAliveTimer = new Timer((object e) =>
            {
                CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_KEEP_ALIVE);
                sendPacket.Push(0);
                Send(sendPacket);
            }, null, 0, 3_000);
        }
        public void StopKeepAlive()
        {
            _keepAliveTimer?.Dispose();
            _keepAliveTimer = null;
        }
    }
}
