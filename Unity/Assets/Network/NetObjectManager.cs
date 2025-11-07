using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using CSharpServerCore;
using Unity.VisualScripting;
using UnityEditor;
using UnityEngine;

public class NetObjectManager : MonoBehaviour
{
    public static NetObjectManager instance;
    public GameObject worldPivot;
    public DiePanel diePanel;
    public DieTrigger dieTrigger;


    public List<HeightObject> heightObjects { get; private set; } =
        new List<HeightObject>((int)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT);
    public Dictionary<UInt64, PlayerMovement> players { get; private set; }
        = new Dictionary<UInt64, PlayerMovement>();




    // Prefab
    public PlayerMovement playerPrefab;
    public BasePivot basePivotPrefab;
    public SlickPivot slickPivotPrefab;
    public BlueEnergy blueEnergyPrefab;
    public LoosePivot loosePivotPrefab;
    public StrongBirdPivot strongBirdPrefab;

    // event
    public event Action<UInt64, Byte> event_ACQUIRE_ATTACH_TO_PIVOT;
    public event Action<UInt64> event_ACQUIRE_JUMP;

    private void Awake()
    {
        if (instance == null)
        {
            instance = this;
        }
        else
        {
            Destroy(gameObject);
            return;
        }

    }
    private void Start()
    {
        for (int i = 0; i < (int)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT; i++)
        {
            heightObjects.Add(null);

        }
    }
    public void Handle_INSTANTIATE_PLAYER(CPacket msg)
    {
        UInt64 ownerId = msg.Pop_UInt64(); // Owner Id   
        InstantiatePlayer(msg, ownerId);
    }



    public void InstantiatePlayer(CPacket msg, UInt64 ownerId)
    {
        UInt16 NetOBjectCode = msg.Pop_UInt16(); // Object Code
        Vector2 pos = new Vector2(msg.Pop_Float(), msg.Pop_Float()); // Pos
        float rot = msg.Pop_Float(); // Rot

        PlayerMovement newPlayer = Instantiate(playerPrefab, pos, Quaternion.Euler(new Vector3(0, 0, rot)));
        newPlayer.transform.SetParent(worldPivot.transform, true);
        newPlayer.Initialize(ownerId);
        players[ownerId] = newPlayer;
        newPlayer.transform.SetParent(worldPivot.transform, true);
    }
    public void LoadLevel(CPacket msg)
    {
        Byte playerCount = msg.Pop_Byte();
        for (int i = 0; i < playerCount; i++)
        {
            UInt64 ownerId = msg.Pop_UInt64();
            InstantiatePlayer(msg, ownerId);
        }

        Byte pivotCount = msg.Pop_Byte();
        for (int i = 0; i < pivotCount; i++)
        {
            Handle_INSTANTIATE_PIVOT(msg);
        }
    }



    public void UpdatePlayerPosRot(CPacket msg)
    {
        PlayerMovement playerMovement = players[msg.Pop_UInt64()];
        if (playerMovement != null)
        {
            playerMovement.SyncRotPos_Others(msg);
        }
    }

    public void Handle_ACQUIRE_ATTACH_TO_PIVOT(CPacket msg)
    {
        UInt64 playerId = msg.Pop_UInt64();
        PlayerMovement player = players[playerId];
        Byte pivotIndex = msg.Pop_Byte();

        if (player.IsMine)
        {

            player.Acquire_AttachToPivot((Pivot)heightObjects[pivotIndex]);
            GameManager.instance.UpdateScore(msg.Pop_UInt32());
            dieTrigger.transform.position = 
                heightObjects[pivotIndex].transform.position + new Vector3(0, -msg.Pop_Float(), 0);
        }

        event_ACQUIRE_ATTACH_TO_PIVOT?.Invoke(playerId, pivotIndex);
    }

    public void Handle_PLAYER_FLIP(CPacket msg)
    {
        PlayerMovement player = players[msg.Pop_UInt64()];
        player.Sync_Flip(msg);
    }

    public void Handle_SPAWN_HEIGHT_OBJECT(CPacket msg)
    {
        HECA heightCategory = (HECA)msg.Pop_Byte();
        switch (heightCategory)
        {
            case HECA.HECA_PIVOT:
                {
                    Handle_INSTANTIATE_PIVOT(msg);
                }
                break;
            case HECA.HECA_ENERGY:
                {
                    Handle_SPAWN_ENERGY(msg);
                }
                break;
            default:
                {
                    Debug.LogError("�Ҵ���� ���� ī�װ��");
                }
                break;
        }
    }

    private void Handle_INSTANTIATE_PIVOT(CPacket msg)
    {
        O objectCode = (O)msg.Pop_UInt16(); // Object Code


        Byte index = msg.Pop_Byte(); // Index
        Vector2 pos = new Vector2(msg.Pop_Float(), msg.Pop_Float()); // Pos
        float rot = msg.Pop_Float(); // Rot

        Pivot newPivot;
        switch (objectCode)
        {
            case O.O_BASE_PIVOT:
                {
                    newPivot = Instantiate(basePivotPrefab, pos,
                        Quaternion.Euler(new Vector3(0, 0, rot)));
                }
                break;
            case O.O_SLICK_PIVOT:
                {
                    newPivot = Instantiate(slickPivotPrefab, pos,
                        Quaternion.Euler(new Vector3(0, 0, rot)));
                }
                break;
            case O.O_LOOSE_PIVOT:
                {
                    newPivot = Instantiate(loosePivotPrefab, pos, Quaternion.Euler(new Vector3(0, 0, rot)));
                }
                break;
            case O.O_STRONG_BIRD:
                {
                    newPivot = Instantiate(strongBirdPrefab, pos, Quaternion.Euler(new Vector3(0, 0, rot)));
                }
                break;
            default:
                {
                    newPivot = null;
                    Debug.LogError("�Ҵ���� ���� ������Ʈ �ڵ�");
                }
                break;
        }

        if (heightObjects[index] != null)
        {
            Debug.LogError("Head Eats Tail");
        }
        heightObjects[index] = newPivot;
        newPivot.Initialize(NetworkManager.instance.playerId, index);
        newPivot.transform.SetParent(worldPivot.transform, true);
    }

    private void Handle_SPAWN_ENERGY(CPacket msg)
    {
        Energy energy;

        O objectCode = (O)msg.Pop_UInt16(); // ObjectCode
        Byte index = msg.Pop_Byte(); // Index

        switch (objectCode)
        {
            case O.O_BLUE_ENERGY:
                {
                    energy = Instantiate(blueEnergyPrefab, new Vector2(msg.Pop_Float(), msg.Pop_Float()),
                        Quaternion.Euler(new Vector3(0, 0, msg.Pop_Float())));
                }
                break;
            default:
                {
                    Debug.LogError("NO_OBJECT_CODE");
                    energy = null;
                }
                break;
        }

        if (heightObjects[index] != null)
        {
            Debug.LogError("Head Eats Tail");
        }
        heightObjects[index] = energy;
        energy.Initialize(NetworkManager.instance.playerId, index, msg.Pop_Byte()); // Amount
        energy.transform.SetParent(worldPivot.transform, true);
    }
    public void Handle_DELETE_HEIGHT_OBJECT(CPacket msg)
    {
        Byte index = msg.Pop_Byte();
        if (heightObjects[index] == null)
        {
            Debug.Log("���� �ǹ��� ���� ��û"); // �� Capsule <-> Multi Room �����, ��Ƽ������ ȯ�濡�� ���� �� ����
            return;
        }

        Destroy(heightObjects[index].gameObject);
        heightObjects[index] = null;
    }
    public void Handle_PLAYER_GAME_OVER(CPacket msg)
    {
        UInt64 dieOwnerId = msg.Pop_UInt64();

        if (dieOwnerId == NetworkManager.instance.playerId)
        {
            PlayerControllerManager.instance.UsingController().gameObject.SetActive(false);
            UInt32 score = msg.Pop_UInt32();
            diePanel.OnGameOver(score);

            if (score > NetworkManager.instance.bestScore)
            {
                NetworkManager.instance.SetBestScore(score);
            }
        }

        PlayerMovement player = players[dieOwnerId];
        if (player != null)
        {
            Destroy(player.gameObject);
            players.Remove(dieOwnerId);
        }
    }

    public void Handle_MERGE_ROOM(CPacket msg)
    {
        Vector2 mineWorldToward = new Vector2(0, 0);

        for(int i = 0; i < 2; i++)
        {
            UInt64 ownerId = msg.Pop_UInt64(); // Owner ID
            Vector2 worldMoveToward = new Vector2(msg.Pop_Float(), msg.Pop_Float()); // Move Toward

            if(ownerId == NetworkManager.instance.playerId)
            {
                Vector2 previousPos = worldPivot.transform.position;
                worldPivot.transform.position = new Vector2(previousPos.x + worldMoveToward.x,
                    previousPos.y + worldMoveToward.y);

                mineWorldToward = worldMoveToward;

                CameraController.instance.DirectMovePos(worldMoveToward);
            }
        }


        for (int i = 0; i < 2; i++)
        {
            UInt64 ownerId = msg.Pop_UInt64(); // Onwer Id
            string playerNickName = msg.Pop_String(); // Nick Name
            UInt32 BestScore = msg.Pop_UInt32(); // BestScore
            byte blueEnergy = msg.Pop_Byte(); // BlueEnergy;

            if (ownerId == NetworkManager.instance.playerId)
            {
                { // ownerId = PlayerId �� �ʿ� ���� �κ�
                    O objectCode = (O)msg.Pop_UInt16(); // Object Code
                    Vector2 playerPos = new Vector2(msg.Pop_Float(), msg.Pop_Float()); // Player Pos
                    float playerRot = msg.Pop_Float(); // Player Rot
                }
            }
            else
            {
                InstantiatePlayer(msg, ownerId);

                GameManager.instance.onGameUIPanel.PostOtherPlayerUIs(playerNickName, BestScore, blueEnergy);
            }
        }


        List<HeightObject> tempList = new List<HeightObject>((int)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT);
        for (int i = 0; i < (int)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT; i++)
        {
            tempList.Add(heightObjects[i]);
        }

#if _Develop
        if (heightObjects.Count != (int)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT)
        {
            Debug.LogError("�ε��� ����ġ");
        }
#endif
        heightObjects.Clear();
        for (int i = 0; i < (int)E.E_MULTI_HEIGHT_POOL_MAX_COUNT; i++)
        {
            heightObjects.Add(null);
        }

        Byte pivotVecSize = msg.Pop_Byte(); // Pivot Vec Size
        for (int i = 0; i < pivotVecSize; i++)
        {
            UInt64 ownerId = msg.Pop_UInt64(); // Owner Id
            Byte previousIndex = msg.Pop_Byte(); // Previous Index

            if (ownerId == NetworkManager.instance.playerId)
            {
                HECA heca = (HECA)msg.Pop_Byte(); // HECA
                switch (heca)
                {
                    case HECA.HECA_PIVOT:
                        {
                            UInt16 objectCode = msg.Pop_UInt16(); // Object Code
                            Byte newIndex = msg.Pop_Byte(); // new Index
                            Vector2 newPos = new Vector2(msg.Pop_Float(), msg.Pop_Float()); // new Pos
                            float rot = msg.Pop_Float(); // Rot


                            heightObjects[newIndex] = tempList[previousIndex];


                            // �� ���� ��Ƽ������ ȯ�濡�� pre �� ���, null �Ҵ�
                            heightObjects[newIndex].SwitchIndex(newIndex);
                            tempList[previousIndex] = null;

                            O pivotObjectCode = heightObjects[newIndex].ObjectCode;
                            if(pivotObjectCode == O.O_STRONG_BIRD)
                            {
                                ((StrongBirdPivot)heightObjects[newIndex]).UpdateCenterPos(mineWorldToward);
                            }

                        }
                        break;
                    case HECA.HECA_ENERGY:
                        {
                            UInt16 objectCode = msg.Pop_UInt16(); // Object Code
                            Byte newIndex = msg.Pop_Byte(); // new Index
                            Vector2 newPos = new Vector2(msg.Pop_Float(), msg.Pop_Float()); // new Pos
                            float rot = msg.Pop_Float(); // Rot
                            Byte amount = msg.Pop_Byte(); // Amount

                            heightObjects[newIndex] = tempList[previousIndex];
                            heightObjects[newIndex].SwitchIndex(newIndex);
                            tempList[previousIndex] = null;
                        }
                        break;
                    default:
                        {
                            Debug.LogError("NOT_ASSIGNED_HECA");
                        }
                        break;
                }
            }
            else
            {
                Handle_SPAWN_HEIGHT_OBJECT(msg);
            }
        }

        for (int i = 0; i < (Byte)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT; i++)
        {
            if (tempList[i] != null)
            {
                Destroy(tempList[i].gameObject);
            }
        }



        CPacket sendMsg = CPacket.PopForCreate((UInt16)C.C_MERGE_ROOM_FINISHED);

        PlayerMovement player = players[NetworkManager.instance.playerId];
        if(player.attachedPivot != null)
        {
            sendMsg.Push((Byte)player.attachedPivot.Index); // attached Index
        }
        else
        {
            sendMsg.Push((Byte)255); // attached Index is Null
        }

        Vector2 playerNewPos = players[NetworkManager.instance.playerId].transform.position;

        sendMsg.Push((float)playerNewPos.x); // posX
        sendMsg.Push((float)playerNewPos.y); // posY

        NetworkManager.instance.Send(sendMsg);
    }

    public void Handle_SPLIT_ROOM(CPacket msg)
    {
        GameManager.instance.onGameUIPanel.CloseOtherPlayerUIs();

        UInt64 destroyingId = 0;
        foreach (var kvp in players)
        {
            if (kvp.Key != NetworkManager.instance.playerId)
            {
                destroyingId = kvp.Key;
            }
        }
        if (players.ContainsKey(destroyingId))
        {
            Destroy(players[destroyingId].gameObject);
            players.Remove(destroyingId);
        }



        List<HeightObject> tempList = new List<HeightObject>((int)E.E_MULTI_HEIGHT_POOL_MAX_COUNT);
        for (int i = 0; i < (int)E.E_MULTI_HEIGHT_POOL_MAX_COUNT; i++)
        {
            tempList.Add(heightObjects[i]);
        }
        heightObjects.Clear();
        for (int i = 0; i < (int)E.E_CAPSULE_HEIGHT_POOL_MAX_COUNT; i++)
        {
            heightObjects.Add(null);
        }


        Byte pivotLength = msg.Pop_Byte(); // Pivot Length


        for (int i = 0; i < pivotLength; i++)
        {
            Byte previousIndex = msg.Pop_Byte(); // Previous Index
            Byte newIndex = msg.Pop_Byte(); // New Index

            heightObjects[newIndex] = tempList[previousIndex];
            heightObjects[newIndex].SwitchIndex(newIndex);
            tempList[previousIndex] = null;
        }


        for (int i = 0; i < (int)E.E_MULTI_HEIGHT_POOL_MAX_COUNT; i++)
        {
            if (tempList[i] != null)
            {
                Destroy(tempList[i].gameObject);
            }
        }


        CPacket sendMsg = CPacket.PopForCreate((UInt16)C.C_SPLIT_ROOM_FINISHED);

        PlayerMovement player = players[NetworkManager.instance.playerId];
        if (player.attachedPivot != null)
        {
            sendMsg.Push((byte)player.attachedPivot.Index); // attached Pivot Index
        }
        else
        {
            sendMsg.Push((Byte)255); // 255��, ���� attachedPivot�� null
        }


        NetworkManager.instance.Send(sendMsg);
    }

    public void Handle_SPAWN_HEIGHT_OBJECTS_INCLUDE_ENERGY(CPacket msg)
    {
        // Energy
        Energy energy;

        O objectCode = (O)msg.Pop_UInt16(); // ObjectCode
        Byte index = msg.Pop_Byte(); // Index

        switch (objectCode)
        {
            case O.O_BLUE_ENERGY:
                {
                    energy = Instantiate(blueEnergyPrefab, new Vector2(msg.Pop_Float(), msg.Pop_Float()),
                        Quaternion.Euler(new Vector3(0, 0, msg.Pop_Float())));
                }
                break;
            default:
                {
                    Debug.LogError("NoObjectCode");
                    energy = null;
                }
                break;
        }

        if (heightObjects[index] != null)
        {
            Debug.LogError("Head Eats Tail");
        }
        heightObjects[index] = energy;
        energy.Initialize(NetworkManager.instance.playerId, index, msg.Pop_Byte()); // Amount
        energy.transform.SetParent(worldPivot.transform, true);

        for (int i = 1; i <= 4; i++)
        {
            Handle_INSTANTIATE_PIVOT(msg);
        }
    }
    public void Handle_ACQUIRE_ENERGY(CPacket msg)
    {
        /*
         * PROTOCOL
         	enum EnergyType : UINT8
	{ 
		Null = 0,
		BlueEnergy = 1,
		RedEnergy = 2,
	};
         * */

        byte index = msg.Pop_Byte();
        byte protocol = msg.Pop_Byte();
        UInt64 playerId = msg.Pop_UInt64();
        byte afterAmount = msg.Pop_Byte();

        switch (protocol)
        {
            case 1: // BlueEnergy
                {
                    if (playerId == NetworkManager.instance.playerId)
                    {
                        GameManager.instance.onGameUIPanel.SetBlueEnergyValue(afterAmount);
                    }
                    else
                    {
                        GameManager.instance.onGameUIPanel.SetOtherPlayers_BlueEnergyValue(afterAmount);
                    }
                }
                break;
            default:
                {
                    Debug.LogError("NOT_ASSIGNED");
                }
                break;
        }



        if (heightObjects[index] != null)
        {
            Destroy(heightObjects[index].gameObject);
            heightObjects[index] = null;
        }
        else
        {
            Debug.LogError("NULL" + " // "+ Time.time);
        }
    }

    public void Handle_ACQUIRE_BLUE_SELF_RACKET(CPacket msg)
    {
        UInt64 playerId = msg.Pop_UInt64();
        Byte remainBlueAmount = msg.Pop_Byte();

        if(playerId == NetworkManager.instance.playerId)
        {
            players[playerId].Handle_ACQUIRE_BLUE_SELF_RACKET(msg);
            GameManager.instance.onGameUIPanel.SetBlueEnergyValue(remainBlueAmount);
        }
        else
        {
            GameManager.instance.onGameUIPanel.
                SetOtherPlayers_BlueEnergyValue(remainBlueAmount);
        }
    }
    public void Handle_DEIED_BLUE_RACKET(CPacket msg)
    {
        UInt64 playerId = msg.Pop_UInt64();
        byte blueAmount = msg.Pop_Byte();

        if(NetworkManager.instance.playerId == playerId)
        {
            GameManager.instance.onGameUIPanel.SetBlueEnergyValue(blueAmount);
            players[playerId].Handle_DENIED_BLUE_RACKET();
        }
        else
        {
            GameManager.instance.onGameUIPanel.SetOtherPlayers_BlueEnergyValue(blueAmount);
        }
    }

    public void Handle_SYNC_PLAYER_ANIMATION(CPacket msg)
    {
        UInt64 playerId = msg.Pop_UInt64();
        players[playerId].SyncAnimation(msg);
    }

    public void Handle_BLUE_SIEZE_MODE(UInt64 siegingPlayerId)
    {

        players[siegingPlayerId].Acquire_BlueSiezeMode();
        
    }
    public void Handle_ACQUIRE_CANCEL_SIEZE_MODE(CPacket msg)
    {
        UInt64 cancelingPlayerId = msg.Pop_UInt64();
        if(NetworkManager.instance.playerId == cancelingPlayerId)
        {
            players[cancelingPlayerId].Acquire_Cancel_SiezeMode();
        }
        else
        {
            GameManager.instance.onGameUIPanel.DisableSizeFireMode();
        }
    }

    public void Handle_ACQUIRE_JUMP(CPacket msg)
    {
        UInt64 playerId = msg.Pop_Byte();
        event_ACQUIRE_JUMP?.Invoke(playerId);
    }


    public void Handle_SIEGE_MODE_FIRING(CPacket msg)
    {
        UInt64 siegingPlayerId = msg.Pop_UInt64(); // Sieging Player Id
        UInt64 flyingPlayerId = msg.Pop_UInt64(); // Flying Player Id

        if(NetworkManager.instance.playerId == siegingPlayerId)
        {
            players[siegingPlayerId].SiegeModeFiring_Sieger();
        }
        else if(NetworkManager.instance.playerId == flyingPlayerId) 
        {

            players[flyingPlayerId].SiegeModeFiring_Flyer(players[siegingPlayerId].transform.position);
        }
    }
    public void Handle_DENIED_SIEGE_FIRING(CPacket msg)
    {
        UInt64 siegingPlayerId = msg.Pop_UInt64(); // Sieging Player Id
        UInt64 flyingPlayerId = msg.Pop_UInt64(); // Flying Player Id

        Debug.LogError("DENIED_SIEGE_FIRING"); // �̷� ��찡 ����� ���ٵ�

        if (NetworkManager.instance.playerId == siegingPlayerId)
        {
            players[siegingPlayerId].Acquire_Cancel_SiezeMode();
        }
        else if (NetworkManager.instance.playerId == flyingPlayerId)
        {
            GameManager.instance.onGameUIPanel.DisableSizeFireMode();
        }

    }

    public void Handle_SIEGE_MODE_FIRE(CPacket msg)
    {
        UInt64 siegingPlayerId = msg.Pop_UInt64(); // Sieging Player ID
        byte remainBlueAmount = msg.Pop_Byte(); // Remain Blue AMoun
        UInt64 flyingPlayerId = msg.Pop_UInt64(); // FlyingPlayer ID

        if(NetworkManager.instance.playerId == siegingPlayerId)
        {
            players[siegingPlayerId].SiegeModeFire_Sieger();
            GameManager.instance.onGameUIPanel.SetBlueEnergyValue(remainBlueAmount);
        }
        else if(NetworkManager.instance.playerId == flyingPlayerId)
        {
            Vector2 direction = players[NetworkManager.instance.playerId].attachedPivot.transform.position
                - players[siegingPlayerId].transform.position;

            players[flyingPlayerId].SiegeModeFire_Flyer(direction);
            GameManager.instance.onGameUIPanel.DisableSizeFireMode();
            GameManager.instance.onGameUIPanel.SetOtherPlayers_BlueEnergyValue(remainBlueAmount);
        }
    }

    public void Handle_LOOSE_PIVOT_WOBBLE(CPacket msg)
    {
        byte index = msg.Pop_Byte();
        LoosePivot loosePivot = (LoosePivot)heightObjects[index];
        loosePivot.Wobble();
    }
    public void Handle_LOOSE_PIVOT_DELETE(CPacket msg)
    {
        byte index = msg.Pop_Byte();

        byte droppingPlayerCount = msg.Pop_Byte();

        for(int i = 0; i < droppingPlayerCount; i++)
        {
            UInt64 playerId = msg.Pop_UInt64();
            PS playerState = (PS)msg.Pop_Byte();
            players[playerId].Drop(playerState == PS.PS_FALLING ? true : false);
        }


        LoosePivot loosePivot = (LoosePivot)heightObjects[index];
        loosePivot.Drop();
        heightObjects[index] = null;
    }

    public void Handle_UPDATE_BIRD_POS(CPacket msg)
    {
        byte index = msg.Pop_Byte();
        Vector2 updatedPos = new Vector2(msg.Pop_Float(), msg.Pop_Float());

        O objectCode = heightObjects[index].ObjectCode;
        if(objectCode == O.O_STRONG_BIRD)
        {
            ((StrongBirdPivot)heightObjects[index]).UpdatePos(updatedPos);    
        }
        

    }
}
