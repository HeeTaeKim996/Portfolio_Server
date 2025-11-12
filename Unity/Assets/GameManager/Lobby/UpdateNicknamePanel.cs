using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class UpdateNicknamePanel : MonoBehaviour
{
    public GameObject background;

    public TMP_InputField inputField;
    public Text nickNameText;
    private string nickNameSt;

    public Button postButton;
    public Button okButton;
    public Button exitButton;

    private void Awake()
    {
        postButton.onClick.AddListener(PostPanel);
        okButton.onClick.AddListener(OnOkButtonClicked);
        exitButton.onClick.AddListener(OnExitButtonClicked);

        inputField.onDeselect.AddListener(OnDeselect);
    }
    private void Start()
    {
        background.gameObject.SetActive(false);
    }
    

    private void OnExitButtonClicked()
    {
        background.gameObject.SetActive(false);
    }
    public void PostPanel()
    {
        background.gameObject.SetActive(true);
        nickNameSt = NetworkManager.instance.nickName;
        nickNameText.text = "NickName : " + nickNameSt;
    }

    private void OnDeselect(string newString)
    {
        nickNameSt = newString;
        nickNameText.text = "NickName : " + nickNameSt;
        Debug.Log(nickNameSt);
    }

    private void OnOkButtonClicked()
    {
        if(nickNameSt == NetworkManager.instance.nickName)
        {
        }
        else if(nickNameSt == "")
        {
        }
        else
        {
            CPacket msg = CPacket.PopForCreate((UInt16)C.C_UPDATE_NICK_NAME);
            msg.Push(nickNameSt);
            NetworkManager.instance.Send(msg);
        }
    }
}
