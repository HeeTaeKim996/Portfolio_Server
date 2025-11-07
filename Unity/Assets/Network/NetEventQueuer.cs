using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;

namespace Network 
{
    public enum NetworkEvent
    {
        Connected,
        Disconnected
    }

    public class NetEventQueuer
    {
        private Queue<NetworkEvent> netEvents = new Queue<NetworkEvent>();
        private Queue<CPacket> netMessages = new Queue<CPacket>();
        private object lock_netQueue = new object();

        public bool HasNetEvent()
        {
            lock (lock_netQueue)
            {
                return netEvents.Count > 0;
            }
        }

        public void EnqueueNetEvent(NetworkEvent netEvent)
        {
            lock (lock_netQueue)
            {
                netEvents.Enqueue(netEvent);
            }
        }
        public NetworkEvent DequeueNetEvent()
        {
            lock (lock_netQueue)
            {
                return netEvents.Dequeue();
            }
        }

        public bool HasNetMessage()
        {
            lock (lock_netQueue)
            {
                return netMessages.Count > 0;
            }
        }
        public void EnqueueNetMessage(CPacket msg)
        {
            lock (lock_netQueue)
            {
                netMessages.Enqueue(msg);
            }
        }
        public CPacket DequeNetMessage()
        {
            lock (lock_netQueue)
            {
                return netMessages.Dequeue();
            }
        }

    }
}

