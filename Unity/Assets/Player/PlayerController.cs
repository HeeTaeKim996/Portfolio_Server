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
        settingPanel = FindObjectOfType<SettingPanel>(); // ※ Start 때 해야, 불필요한 서치 줄임
#if _Develop
        if(settingPanel == null)
        {
            Debug.LogError("SettingPanel을 찾을 수 없음");
        }
#endif
    }

    public void SetPlayMode(bool isPlayMode)
    {
        this.isPlayMode = isPlayMode;
    }
}
