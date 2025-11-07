using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class LobbyManager : MonoBehaviour
{
    public static LobbyManager instance;
    [SerializeField]
    private Text lobbyInformText;
    public GameObject lobbyBackground;
    public Button gameStartButton;

    public Text bestScoreText;
    public Text nickNameText;

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

        gameStartButton.onClick.AddListener(OnGameStartButtonClicked);
    }
    private void Start()
    {
        lobbyBackground.gameObject.SetActive(false);
        if (NetworkManager.instance.isConnected)
        {
            OnConnected_ToServer();
        }
    }

    public void OnConnected_ToServer()
    {
        lobbyBackground.gameObject.SetActive(true);
        lobbyInformText.text = "Connected To Lobby";

        bestScoreText.text = "BestScore : " + NetworkManager.instance.bestScore.ToString();
        nickNameText.text = "NickName : " + NetworkManager.instance.nickName;
    }
    
    private void OnGameStartButtonClicked()
    {
        CPacket msg = CPacket.PopForCreate((UInt16)C.C_MAKE_ROOM);
        NetworkManager.instance.Send(msg);
    }

    public void UpdateNicknameText(string nickName)
    {
        nickNameText.text = "NickName : " + nickName;
    }

}
