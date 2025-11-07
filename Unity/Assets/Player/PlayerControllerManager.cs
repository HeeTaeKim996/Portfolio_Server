using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerControllerManager : MonoBehaviour
{
    public static PlayerControllerManager instance;
    public bool ISMobleSimulating;

    private PlayerController _usingController;
    private PlayerController_PC _playerController_PC;
    private PlayerController_Mobile _playerController_Mobile;
    public GameObject mobileUIPanel;

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


        _playerController_PC = GetComponentInChildren<PlayerController_PC>();
        _playerController_Mobile = GetComponentInChildren<PlayerController_Mobile>();

        if((Application.platform == RuntimePlatform.Android || Application.platform == RuntimePlatform.IPhonePlayer)
            || ISMobleSimulating)
        {
            _usingController = _playerController_Mobile;
            Destroy(_playerController_PC.gameObject);
            mobileUIPanel.GetComponent<CanvasGroup>().alpha = 1;
        }
        else
        {
            _usingController = _playerController_PC;
            Destroy(_playerController_Mobile.gameObject);
            mobileUIPanel.gameObject.SetActive(false);
        }
    }

    public PlayerController UsingController()
    {
        return _usingController;
    }
}
