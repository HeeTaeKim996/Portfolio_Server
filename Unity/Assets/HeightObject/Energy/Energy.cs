using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Energy : HeightObject
{
    protected override void Awake()
    {
        base.Awake();
    }
    protected override void Start()
    {
        base.Start();
    }


    protected byte _amount;
    
    public enum EnergyType
    {
        BlueEnergy,
        RendEnergy
    }
    public EnergyType energyType { get; protected set; }

    
    public byte Amount
    {
        get
        {
            return _amount;
        }
    }

    public void Initialize(UInt64 ownerId, byte index, byte amount)
    {
        base.Initialize(ownerId, index);
        _amount = amount;
    }
}
