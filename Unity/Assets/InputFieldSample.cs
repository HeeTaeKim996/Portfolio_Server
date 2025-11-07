using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class InputFieldSample : MonoBehaviour
{
    public TMP_InputField inputField;


    private void Start()
    {
        inputField.onDeselect.AddListener(OnDeselect);
    }

    private void OnDeselect(string inputStr)
    {
        inputField.text = "";

        //CPacket sendPacket = CPacket.PopForCreate((UInt16)C.C_TEST_CHAT);
        //sendPacket.Push(inputStr);
        //NetworkManager.instance.Send(sendPacket);
    }
}
