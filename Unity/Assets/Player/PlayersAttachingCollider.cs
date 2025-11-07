using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayersAttachingCollider : MonoBehaviour
{
    private PlayerMovement playerMovement;
    public LayerMask pivotLayer { get; private set; }

    private bool isMine;

    private void Awake()
    {
        playerMovement = GetComponentInParent<PlayerMovement>();
        pivotLayer = LayerMask.GetMask("Pivot");
    }
    private void Start()
    {
        
    }

    public void SetIsMine(bool isMine)
    {
        this.isMine = isMine;
    }


    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (!isMine) return;

        int collisionLayer = 1 << collision.gameObject.layer;

        if ((collisionLayer & pivotLayer) != 0)
        {
            playerMovement.Require_AttachToPivot(collision.gameObject.GetComponent<Pivot>());
        }
    }
}
