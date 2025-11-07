using System;
using System.Collections.Generic;
using System.Text;

namespace CSharpServerCore
{
    public abstract class Session
    {
        public abstract void OnMessage(ConstBuffer buffer);
        public abstract void OnRemoved();
        public abstract void Send(CPacket msg);
        public abstract void Disconnect();
    }
}
