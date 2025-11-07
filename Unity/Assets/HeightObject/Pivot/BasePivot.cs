using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BasePivot : Pivot
{
    protected override void Awake()
    {
        base.Awake();
        _objectCode = O.O_BASE_PIVOT;
    }
    protected override void Start()
    {
        base.Start();

    }
}
