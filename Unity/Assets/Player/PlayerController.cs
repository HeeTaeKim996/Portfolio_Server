using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    protected PlayerMovement playerMovement;
    protected SettingPanel settingPanel;
    protected bool isPlayMode = true;
    public void SetPlayer(PlayerMovement playerMovement)
    {
        this.playerMovement = playerMovement;
    }

    protected virtual void Awake()
    {

    }

    protected virtual void Start()
    {
        settingPanel = FindObjectOfType<SettingPanel>();
#if _Develop
        if(settingPanel == null)
        {
        }
#endif
    }

    public void SetPlayMode(bool isPlayMode)
    {
        this.isPlayMode = isPlayMode;
    }
}
