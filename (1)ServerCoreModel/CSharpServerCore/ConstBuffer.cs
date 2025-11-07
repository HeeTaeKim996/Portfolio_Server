using System;
using System.Collections.Generic;
using System.Text;

namespace CSharpServerCore
{
    public struct ConstBuffer 
    {
        public byte[] Value { get; private set; }
        public int position { get; private set; }

        public ConstBuffer(byte[] value, int position)
        {
            this.Value = value;
            this.position = position;
        }
    }

}
