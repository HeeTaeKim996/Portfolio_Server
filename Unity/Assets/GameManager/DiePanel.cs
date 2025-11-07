using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using CSharpServerCore;
using UnityEngine;
using UnityEngine.UI;

public class DiePanel : MonoBehaviour
{
    [SerializeField]
    private GameObject background;
    [SerializeField]
    private Text scoreText;
    [SerializeField]
    private Button backToLobbyButton;

    private void Awake()
    {
        backToLobbyButton.onClick.AddListener(OnBackToLobbyButtonClicked);
        background.GetComponent<CanvasGroup>().alpha = 1;
        background.gameObject.SetActive(false);
    }


    public void OnGameOver(UInt32 score)
    {
        background.gameObject.SetActive(true);
        scoreText.text = "Score : "  + score.ToString();
    }

    public void OnBackToLobbyButtonClicked()
    {
        CPacket msg = CPacket.PopForCreate((UInt16)C.C_BACK_TO_LOBBY);
        NetworkManager.instance.Send(msg);
    }
}
