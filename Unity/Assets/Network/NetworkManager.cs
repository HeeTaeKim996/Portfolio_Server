using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using CSharpServerCore;
using Network;
using UnityEngine;
using UnityEngine.SceneManagement;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager instance;
    private CNetUnityService cNetUnityService;

    public string remoteEndPoint;
    public UInt64 playerId { get; private set; }

    public UInt32 bestScore { get; private set; }
    public string nickName { get; private set; }

    string loginPath;

    private void Awake()
    {
        if (instance == null)
        {
            instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            Destroy(gameObject);
        }

        if (instance != this) return;
        // �� Destroy �� ����ǵ�, Awake,Start �����

        cNetUnityService = gameObject.AddComponent<CNetUnityService>();
        cNetUnityService.Connect(remoteEndPoint, 7777);
    }

    private void Start()
    {
        if (instance != this) return;
    }


    public bool isConnected { get; private set; } = false;

    public void OnConnected_ToServer()
    {
        loginPath = Application.persistentDataPath + "login_id.json";

        { // DEBUG_NEW_ID
            //File.Delete(loginPath);
        }

        if (!File.Exists(loginPath))
        {
            CPacket msg = CPacket.PopForCreate((UInt16)C.C_CREATE_ID);
            Send(msg);
        }
        else
        {
            string loginId = File.ReadAllText(loginPath);


            CPacket msg = CPacket.PopForCreate((UInt16)C.C_ENTER_GAME);
            msg.Push(loginId);
            Send(msg);
        }

    }


    public void OnNetStatusChanged(NetworkEvent netEvent)
    {
        switch (netEvent)
        {
            case NetworkEvent.Connected:
                {
                    Debug.Log("Connected To Server");
                    isConnected = true;

                    OnConnected_ToServer();
                }
                break;

            case NetworkEvent.Disconnected:
                {
                    Debug.Log("Disconnected From Server");
                    isConnected = false;
                }
                break;
        }
    }

    public void OnSceneLoadFinished()
    {
        string currentSceneName = SceneManager.GetActiveScene().name;
        if (currentSceneName == "MainGame")
        {
            CPacket msg = CPacket.PopForCreate((UInt16)C.C_ENTER_ROOM_FINISHED);
            Send(msg);
        }
        else if (currentSceneName == "Lobby")
        {
            CPacket msg = CPacket.PopForCreate((UInt16)C.C_ENTER_LOBBY_FINISHED);
            Send(msg);
        }
    }

    public void OnMessage(CPacket msg)
    {
        S s = (S)msg.Pop_UInt16();

        switch (s)
        {
            case S.S_CREATE_ID:
                {
                    string loginId = msg.Pop_String();
                    if (File.Exists(loginPath))
                    {
                        Debug.LogError("�̹� ������ �ִµ� �������̵� �õ�?");
                    }

                    File.WriteAllText(loginPath, loginId);

                    string loginId_ForCheck = File.ReadAllText(loginPath);

                    CPacket sendMsg = CPacket.PopForCreate((UInt16)C.C_ENTER_GAME);
                    sendMsg.Push(loginId_ForCheck);
                    Send(sendMsg);
                }
                break;
            case S.S_LOGIN_FAILED:
                {
                    Debug.LogError("�α��� ����");
                }
                break;
            case S.S_ENTER_GAME:
                {
                    playerId = msg.Pop_UInt64();
                    bestScore = msg.Pop_UInt32();
                    nickName = msg.Pop_String();
                    if (LobbyManager.instance != null)
                    {
                        LobbyManager.instance.OnConnected_ToServer();
                    }
                }
                break;
            case S.S_ENTER_ROOM:
                {
                    SceneManager.LoadScene("MainGame");
                }
                break;
            case S.S_INSTANTIATE_PLAYER:
                {
                    NetObjectManager.instance.Handle_INSTANTIATE_PLAYER(msg);
                }
                break;
            case S.S_PLAYER_UPDATE_ROT_POS:
                {
                    NetObjectManager.instance.UpdatePlayerPosRot(msg);
                }
                break;

            case S.S_LOAD_LEVEL:
                {
                    NetObjectManager.instance.LoadLevel(msg);
                }
                break;
            case S.S_PLAYER_GAME_OVER:
                {
                    NetObjectManager.instance.Handle_PLAYER_GAME_OVER(msg);
                }
                break;
            case S.S_ACQUIRE_ATTACH_TO_PIVOT:
                {
                    NetObjectManager.instance.Handle_ACQUIRE_ATTACH_TO_PIVOT(msg);
                }
                break;
            case S.S_PLAYER_FLIP:
                {
                    NetObjectManager.instance.Handle_PLAYER_FLIP(msg);
                }
                break;
            case S.S_SPAWN_HEIGHT_OBJECT:
                {
                    NetObjectManager.instance.Handle_SPAWN_HEIGHT_OBJECT(msg);
                }
                break;
            case S.S_DELETE_HEIGHT_OBJECT:
                {
                    NetObjectManager.instance.Handle_DELETE_HEIGHT_OBJECT(msg);
                }
                break;
            case S.S_ENTER_LOBBY:
                {
                    if (SceneManager.GetActiveScene().name != "Lobby")
                    {
                        SceneManager.LoadScene("Lobby");
                    }
                }
                break;
            case S.S_MERGE_ROOM:
                {
                    NetObjectManager.instance.Handle_MERGE_ROOM(msg);
                }
                break;
            case S.S_SPLIT_ROOM:
                {
                    NetObjectManager.instance.Handle_SPLIT_ROOM(msg);
                }
                break;
            case S.S_UPDATE_NICK_NAME:
                {
                    nickName = msg.Pop_String();
                    LobbyManager.instance.UpdateNicknameText(nickName);
                }
                break;
            case S.S_SPAWN_HEIGHT_OBJECTS_INCLUDE_ENERGY:
                {
                    NetObjectManager.instance.
                        Handle_SPAWN_HEIGHT_OBJECTS_INCLUDE_ENERGY(msg);
                }
                break;
            case S.S_ACQUIRE_JUMP:
                {
                    NetObjectManager.instance.Handle_ACQUIRE_JUMP(msg);
                }
                break;
            case S.S_ACQUIRE_ENERGY:
                {
                    NetObjectManager.instance.Handle_ACQUIRE_ENERGY(msg);
                }
                break;
            case S.S_ACQUIRE_BLUE_SELF_RACKET:
                {
                    NetObjectManager.instance.Handle_ACQUIRE_BLUE_SELF_RACKET(msg);
                }
                break;
            case S.S_DENY_BLUE_RACKET:
                {
                    GameManager.instance.onGameUIPanel.OnDeniedBlueEnergyUsing();
                    NetObjectManager.instance.Handle_DEIED_BLUE_RACKET(msg);
                }
                break;
            case S.S_SYNC_PLAYER_ANIMATION:
                {
                    NetObjectManager.instance.Handle_SYNC_PLAYER_ANIMATION(msg);
                }
                break;
            case S.S_BLUE_SIEZE_MODE:
                {
                    UInt64 siegingPlayerId = msg.Pop_UInt64();
                    if(NetworkManager.instance.playerId == siegingPlayerId)
                    {
                        NetObjectManager.instance.Handle_BLUE_SIEZE_MODE(siegingPlayerId);
                    }
                    else
                    {
                        byte attahcedPivotIndex = msg.Pop_Byte();
                        GameManager.instance.onGameUIPanel.
                            EnableSiezeFireMode(siegingPlayerId, attahcedPivotIndex);
                    }

                }
                break;
            case S.S_ACQUIRE_CANCEL_SIEZE_MODE:
                {
                    NetObjectManager.instance.Handle_ACQUIRE_CANCEL_SIEZE_MODE(msg);
                }
                break;

            case S.S_BLUE_SIEZE_MODE_FIRING:
                {
                    NetObjectManager.instance.Handle_SIEGE_MODE_FIRING(msg);
                }
                break;
            case S.S_DENIED_SIEGE_FIRING:
                {
                    NetObjectManager.instance.Handle_DENIED_SIEGE_FIRING(msg);
                }
                break;

            case S.S_BLUE_SEIGE_MODE_fIRE:
                {
                    NetObjectManager.instance.Handle_SIEGE_MODE_FIRE(msg);
                }
                break;
            case S.S_LOOSE_PIVOT_WOBBLE:
                {
                    NetObjectManager.instance.Handle_LOOSE_PIVOT_WOBBLE(msg);
                }
                break;
            case S.S_LOOSE_PIVOT_DELETE:
                {
                    NetObjectManager.instance.Handle_LOOSE_PIVOT_DELETE(msg);
                }
                break;
            case S.S_UPDATE_BIRD_POS:
                {
                    NetObjectManager.instance.Handle_UPDATE_BIRD_POS(msg);
                }
                break;

            default:
                {
                    Debug.LogError($"Not Assigned Msg : {s}");
                }
                break;
        }
    }

    public void Send(CPacket msg)
    {
        cNetUnityService.Send(msg);
    }

    public void SetBestScore(UInt32 score)
    {
        bestScore = score;
    }
}
