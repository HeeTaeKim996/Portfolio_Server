using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using UnityEngine;

public class Pivot : HeightObject
{
    public override void Initialize(UInt64 ownerId, byte index)
    {
        base.Initialize(ownerId, index);
    }


    protected override void Awake()
    {
        base.Awake();
    }
    protected override void Start()
    {
        base.Start();
    }

    public virtual Vector2 Pos()
    {
        return transform.position;
    }

    public virtual void UpdateCenterPos(Vector2 movePos){}
}
