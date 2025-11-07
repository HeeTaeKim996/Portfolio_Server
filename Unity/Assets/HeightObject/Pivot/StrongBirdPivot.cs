using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StrongBirdPivot : Pivot
{
    protected override void Awake()
    {
        base.Awake();
        _objectCode = O.O_STRONG_BIRD;
        _centerPos = transform.position;
    }
    protected override void Start()
    {
        base.Start();
    }

    private Vector2 _centerPos;
    public override Vector2 Pos()
    {
        return _centerPos;
    }

    public override void UpdateCenterPos(Vector2 movePos)
    {
        _centerPos += movePos;
    }


    public void UpdatePos(Vector2 updatedPos)
    {
        transform.position = updatedPos;
    }
}
