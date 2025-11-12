using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LoosePivot : Pivot
{
    protected override void Awake()
    {
        base.Awake();
        _objectCode = O.O_LOOSE_PIVOT;
    }
    protected override void Start()
    {
        base.Start();
    }

    public void Wobble()
    {
    }

    public void Drop()
    {
        Destroy(gameObject);
    }

}
