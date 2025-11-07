using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SettingPanel : MonoBehaviour
{
    private PlayerController playerController;
    public GameObject background;
    public Button exitButton;

    private void Awake()
    {
        background.GetComponent<CanvasGroup>().alpha = 1;
        exitButton.onClick.AddListener(CloseSettingPanel);
    }


    private void Start()
    {
        background.gameObject.SetActive(false);
        playerController = FindObjectOfType<PlayerControllerManager>().UsingController();
    }

    public void OpenSettingPanel()
    {
        background.gameObject.SetActive(true);
    }

    public void CloseSettingPanel()
    {
        background.gameObject.SetActive(false);
        playerController.SetPlayMode(true);
    }
}
