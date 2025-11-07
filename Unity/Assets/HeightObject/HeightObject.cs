using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class HeightObject : NetObject
{

    protected override void Awake()
    {
        base.Awake();
    }

    protected override void Start()
    {
        base.Start();
    }

    protected byte _index;

    public byte Index
    {
        get
        {
            return _index;
        }
    }

    public virtual void Initialize(UInt64 ownerId, byte index)
    {
        base.Initialize(ownerId);
        _index = index;
    }

    public void SwitchIndex(byte index)
    {
        _index = index;
    }

    protected O _objectCode = O.O_NOT_ASSIGNED;

    public O ObjectCode
    {
        get
        {
            return _objectCode;
        }
    }

}
