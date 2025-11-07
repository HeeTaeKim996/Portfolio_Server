using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static GameManager instance;
    public OnGameUIPanel onGameUIPanel { get; private set; }
    

#if _Develop
    public bool isCantFailMode { get; private set; } = false;
#endif

    public UInt32 score { get; private set; }
    public PlayerMovement player { get; private set; }


    private void Awake()
    {
        if(instance == null)
        {
            instance = this;
        }
        else
        {
            Destroy(gameObject);
            return;
        }

        Physics2D.gravity = new Vector2(0, -22f);

        onGameUIPanel = GetComponentInChildren<OnGameUIPanel>();
    }
    private void Start()
    {
        onGameUIPanel.CloseOtherPlayerUIs();
    }

#if _Develop
    public void SetCantFailMode(bool isCantFailMode)
    {
        this.isCantFailMode = isCantFailMode;
    }
#endif

    public void UpdateScore(UInt32 score)
    {
        this.score = score;
        onGameUIPanel.SetScoreText(score);
        
    }

    public void SetPlayer(PlayerMovement playerMovement)
    {
        this.player = playerMovement;
    }
}
