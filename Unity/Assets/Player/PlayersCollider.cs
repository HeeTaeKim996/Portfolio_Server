using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using UnityEngine;

public class PlayersCollider : MonoBehaviour
{
    private PlayerMovement player;

    public UInt64 OwnerId
    {
        get
        {
            return player.OwnerId;
        }
    }

    private LayerMask energyLayer;

    

    private void Awake()
    {
        player = GetComponentInParent<PlayerMovement>();
        energyLayer = LayerMask.GetMask("Energy");
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (((1 << collision.gameObject.layer) & energyLayer) != 0)
        {
            player.OnEnergyLayerCollide(collision.GetComponent<Energy>());
        }
    }
}
