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
        Debug.Log("Wobble 애니매이션 예정");
    }

    public void Drop()
    {
        Debug.Log("Drop 애니매이션 예정");
        Destroy(gameObject);
    }

}
