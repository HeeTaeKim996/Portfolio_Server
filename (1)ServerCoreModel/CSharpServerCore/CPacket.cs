using System;
using System.Data;
using System.Text;
using System.Xml.Schema;

namespace CSharpServerCore
{
    public class CPacket
    {
        public byte[] buffer { get; private set; } = new byte[StaticValues.bufferSize];

        public UInt16 pos { get; private set; }

        private void CopyBuffer(byte[] copiedBuffer, int copiedPos)
        {
            Buffer.BlockCopy(copiedBuffer, 0, buffer, 0, copiedPos);
        }
        private void SetPosition(UInt16 setPos)
        {
            pos = setPos;
        }


        public static CPacket PopForRead(ConstBuffer constBuffer)
        {
            CPacket retPacket = CPacketBufferManager.Pop();
            retPacket.CopyBuffer(constBuffer.Value, constBuffer.position);
            retPacket.SetPosition(StaticValues.packetLenSize);
            return retPacket;
        }
        public static CPacket PopForSend(CPacket copiedPacket, UInt16 copiedPos)
        {
            CPacket retPacket = CPacketBufferManager.Pop();
            retPacket.CopyBuffer(copiedPacket.buffer, copiedPos);
            retPacket.SetPosition(copiedPos);
            return retPacket;
        }
        public static CPacket PopForCreate(UInt16 id)
        {
            CPacket retPacket = CPacketBufferManager.Pop();

            Buffer.BlockCopy(BitConverter.GetBytes(id), 0, retPacket.buffer, 2, 2);

            retPacket.pos = 4;

            return retPacket;
        }
        public void RecordSize()
        {
            Buffer.BlockCopy(BitConverter.GetBytes((UInt16)pos), 0, buffer, 0, 2);
        }


        public static void PushBack(CPacket packet)
        {
            CPacketBufferManager.Push(packet);
        }





        public byte Pop_Byte()
        {
            byte data = buffer[pos];
            pos += sizeof(byte);
            return data;
        }
        public Int16 Pop_Int16()
        {
            Int16 data = BitConverter.ToInt16(buffer, pos);
            pos += sizeof(Int16);
            return data;
        }
        public UInt16 Pop_UInt16()
        {
            UInt16 data = BitConverter.ToUInt16(buffer, pos);
            pos += sizeof(UInt16);
            return data;
        }
        public Int32 Pop_Int32()
        {
            Int32 data = BitConverter.ToInt32(buffer, pos);
            pos += sizeof(Int32);
            return data;
        }
        public UInt32 Pop_UInt32()
        {
            UInt32 data = BitConverter.ToUInt32(buffer, pos);
            pos += sizeof(UInt32);
            return data;
        }
        public Int64 Pop_Int64()
        {
            Int64 data = BitConverter.ToInt64(buffer, pos);
            pos += sizeof(Int64);
            return data;
        }
        public UInt64 Pop_UInt64()
        {
            UInt64 data = BitConverter.ToUInt64(buffer, pos);
            pos += sizeof(UInt64);
            return data;
        }
        public float Pop_Float()
        {
            float data = BitConverter.ToSingle(buffer, pos);
            pos += sizeof(Single);
            return data;
        }
        public string Pop_String()
        {
            UInt16 len = BitConverter.ToUInt16(buffer, pos);
            pos += sizeof(UInt16);

            string data = Encoding.Unicode.GetString(buffer, pos, len);
            pos += len;

            return data;
        }




        public void Push(byte data)
        {
            buffer[pos] = data;
            pos += sizeof(byte);
        }
        public void Push(Int16 data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(Int16));
            pos += sizeof(Int16);
        }
        public void Push(UInt16 data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(UInt16));
            pos += sizeof(UInt16);
        }
        public void Push(Int32 data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(Int32));
            pos += sizeof(Int32);
        }
        public void Push(UInt32 data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(UInt32));
            pos += sizeof(UInt32);
        }
        public void Push(Int64 data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(Int64));
            pos += sizeof(Int64);
        }
        public void Push(UInt64 data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(UInt64));
            pos += sizeof(UInt64);
        }
        public void Push(float data)
        {
            byte[] pushBuffer = BitConverter.GetBytes(data);
            Buffer.BlockCopy(pushBuffer, 0, buffer, pos, sizeof(Single));
            pos += sizeof(Single);
        }
        public void Push(string data)
        {
            byte[] dataBuffer = Encoding.Unicode.GetBytes(data);
            UInt16 len = (UInt16)dataBuffer.Length;
            byte[] lenBuffer = BitConverter.GetBytes(len);

            Buffer.BlockCopy(lenBuffer, 0, buffer, pos, sizeof(UInt16));
            pos += sizeof(UInt16);

            Buffer.BlockCopy(dataBuffer, 0, buffer, pos, len);
            pos += len;
        }



        public UInt16 Reserve()
        {
            pos += sizeof(UInt16);
            return pos;
        }

        public void CheckIn(UInt16 writeStartPos)
        {
            byte[] pushBuffer = BitConverter.GetBytes((UInt16)( pos - writeStartPos ));
            Buffer.BlockCopy(pushBuffer, 0, buffer, writeStartPos - sizeof(UInt16), sizeof(UInt16));
        }
    }
    public struct PacketReserver : IDisposable
    {
        public CPacket packet;
        private UInt16 writeStartPos;

        public PacketReserver(CPacket packet)
        {
            this.packet = packet;
            writeStartPos = packet.Reserve();
        }

        public void Dispose()
        {
            packet.CheckIn(writeStartPos);
        }
    }
}

