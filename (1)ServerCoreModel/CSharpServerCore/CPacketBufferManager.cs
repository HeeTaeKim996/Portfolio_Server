using System;
using System.Collections.Generic;
using System.Threading;


namespace CSharpServerCore
{
    public class CPacketBufferManager
    {
        private static int _poolCapacity;
        private static Stack<CPacket> _cPacketPool;
        private static object lock_packetPool = new object();

#if __CPACKET_POOL_COUNT_CHECK

#endif
        public delegate void PoolCountChecker(int poolCount); // FOR_DEBUG
        public static PoolCountChecker poolCountChecker; // FOR_DEBUG



        public static void Initialize(int capacity)
        {
            _poolCapacity = capacity;
            _cPacketPool = new Stack<CPacket>(capacity);
            Allocate();
        }

        private static void Allocate()
        {
            lock (lock_packetPool)
            {
                for(int i = 0; i < _poolCapacity; i++)
                {
                    _cPacketPool.Push(new CPacket());
                }
            }
        }

        public static void Push(CPacket packet)
        {
            lock (lock_packetPool)
            {
                _cPacketPool.Push(packet);

#if __CPACKET_POOL_COUNT_CHECK
                poolCountChecker?.Invoke(Count);
#endif
            }
        }

        public static CPacket Pop()
        {
            if(_cPacketPool.Count <= 0)
            {
                Allocate();
            }
            lock (lock_packetPool)
            {
                return _cPacketPool.Pop();
            }
        }

        public static int Count
        {
            get
            {
                return _cPacketPool.Count;
            }
        }
    }
}
