using System;
using System.Collections.Generic;
using System.Text;

namespace CSharpServerCore
{
    internal class TcpMsgResolver
    {
        private byte[] _messageBuffer = new byte[StaticValues.bufferSize];
        private int _remainBytes;
        private int _quantityToRead = 0;
        private int _currentReaded = 0;

        public delegate void CompleteMessageCallback(ConstBuffer buffer);

        public void OnReceiveTcp(byte[] TcpBuffer, int offset, int transferred, 
            CompleteMessageCallback callback)
        {
            _remainBytes = transferred;
            int srcPosition = offset;

            while(_remainBytes > 0)
            {
                bool completed;

                if(_currentReaded < 2)
                {
                    _quantityToRead = 2;

                    completed = ReadUntil(TcpBuffer, ref srcPosition, offset, transferred);
                    if (!completed) return;

                    _quantityToRead = (int)BitConverter.ToUInt16(_messageBuffer);
                    // ※ C++서버에서는 header를 UInt16으로 줌. 
                    //   Buffer.BlockCopy는 정수 인자를 uint16이 아닌 int로 받기 때문에, 위처럼 형변환
                }

                completed = ReadUntil(TcpBuffer, ref srcPosition, offset, transferred);
                if (completed)
                {
                    callback(new ConstBuffer(_messageBuffer, _currentReaded));
                    ClearBuffer();
                }
            }
        }

        public bool ReadUntil(byte[] tcpBuffer, ref int srcPosition, int offset, int transferred)
        {
            if (srcPosition >= offset + transferred) return false;

            int readSize = _quantityToRead - _currentReaded;
            if(_remainBytes < readSize)
            {
                readSize = _remainBytes;
            }

            Buffer.BlockCopy(tcpBuffer, srcPosition, _messageBuffer, _currentReaded, readSize);

            _remainBytes -= readSize;
            srcPosition += readSize;
            _currentReaded += readSize;

            if (_currentReaded < _quantityToRead) return false;
            
            return true;
        }

        private void ClearBuffer()
        {
            Array.Clear(_messageBuffer, 0, _messageBuffer.Length);
            _currentReaded = 0;
        }
    }
}
