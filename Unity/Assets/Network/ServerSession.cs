using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;



namespace Network
{
    public class ServerSession : Session
    {
        public CUserToken token { get; private set; }
        private WeakReference weakRef_netEventQueuer;

        public ServerSession(CUserToken token)
        {
            this.token = token;
            token.SetSession(this);
        }
        public void SetNetEventQueuer(NetEventQueuer netEventQueuer)
        {
            weakRef_netEventQueuer = new WeakReference(netEventQueuer);
        }

        public override void OnMessage(ConstBuffer buffer)
        {
            CPacket recvMsg = CPacket.PopForRead(buffer);
            (weakRef_netEventQueuer.Target as NetEventQueuer).EnqueueNetMessage(recvMsg);
        }
        public override void Send(CPacket msg)
        {
            token.Send(msg);
        }
        public override void OnRemoved()
        {
            (weakRef_netEventQueuer.Target as NetEventQueuer).EnqueueNetEvent(NetworkEvent.Disconnected);
        }
        public override void Disconnect()
        {
            
        }
    }

}
