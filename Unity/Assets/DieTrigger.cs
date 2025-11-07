using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;

public class DieTrigger : MonoBehaviour
{
    private LayerMask playerLayer;
    private void Awake()
    {
        playerLayer = LayerMask.GetMask("Player");
    }


    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (((1 << collision.gameObject.layer) & playerLayer) != 0)
        {
            UInt64 ownerId = (UInt64)collision.gameObject.GetComponent<PlayersCollider>().OwnerId;

            if(NetworkManager.instance.playerId == ownerId)
            {
                CPacket msg = CPacket.PopForCreate((UInt16)C.C_PLAYER_DIE_TRIGGERED);
                msg.Push(ownerId);

                NetworkManager.instance.Send(msg);
            }
        }
    }
}
