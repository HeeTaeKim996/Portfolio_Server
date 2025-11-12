using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;

public class NetObject : MonoBehaviour
{
    protected  UInt16 netEnum;

    protected UInt64 ownerId;
    protected bool isMine;

    protected virtual void Awake()
    {

    }
    protected virtual void Start()
    {

    }


    public virtual void Initialize(UInt64 ownerId)
    {
        this.ownerId = ownerId;
        isMine = NetworkManager.instance.playerId == ownerId;
    }

    public bool IsMine
    {
        get
        {
            return isMine;
        }
    }
    public UInt64 OwnerId
    {
        get
        {
            return ownerId;
        }
    }
}