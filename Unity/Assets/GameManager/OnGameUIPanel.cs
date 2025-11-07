using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;
using UnityEngine.UI;

public class OnGameUIPanel : MonoBehaviour
{
    [SerializeField]
    private Text scoreText;


    public Slider blueEnergySlider;
    [SerializeField]
    private Button blueEnergyButton;
    private Coroutine blueEnergyCoroutine;


    [SerializeField]
    private GameObject otherPlayerUIs;
    public Text otherPlayerInfoText;
    [SerializeField]
    private Slider otherPlayerBlueEnergySlider;
    private Coroutine otherPlayerBlueEnergyCoroutine;


    public Button siegeFireButton;
    private Image siegeFireButtonImage;
    private UInt64 siegingPlayerId;
    private byte siegingPlayersPivotIndex;

    private void Awake()
    {
        blueEnergyButton.onClick.AddListener(OnBlueEnergyButtonClicked);
        blueEnergySlider.minValue = 0;
        blueEnergySlider.maxValue = 100;
        blueEnergySlider.value = 100;

        otherPlayerBlueEnergySlider.minValue = 0;
        otherPlayerBlueEnergySlider.maxValue = 100;
        otherPlayerBlueEnergySlider.value = 0;

        siegeFireButton.onClick.AddListener(OnSiegeFireButtonClicked);
        siegeFireButtonImage = siegeFireButton.GetComponent<Image>();
    }
    private void Start()
    {
        if(blueEnergySlider.value < 50)
        {
            blueEnergyButton.enabled = false;
        }

        siegeFireButton.gameObject.SetActive(false);
    }
    private void OnDestroy()
    {
        NetObjectManager.instance.event_ACQUIRE_ATTACH_TO_PIVOT -= On_ACQUIRE_ATTACH_TO_PIVOT;
        NetObjectManager.instance.event_ACQUIRE_JUMP -= On_ACQUIRE_JUMP;
    }

    public void SetScoreText(UInt32 score)
    {
        scoreText.text = "Score : " + score;
    }

    public void PostOtherPlayerUIs(string otherPlayerName, UInt32 otherPlayersScore, 
        byte blueEnergy)
    {
        otherPlayerUIs.gameObject.SetActive(true);
        otherPlayerInfoText.text = otherPlayerName + "(" + otherPlayersScore.ToString()
            + ")";
        otherPlayerBlueEnergySlider.value = blueEnergy;
    }
    public void CloseOtherPlayerUIs()
    {
        otherPlayerUIs.gameObject.SetActive(false);
    }


    private void OnBlueEnergyButtonClicked()
    {
        GameManager.instance.player.OnBlueEnergyButtonClicked();
    }
    public void OnBlueEnergyUsing()
    {
        Debug.Log("블루에너지버튼활성화 UI 추가 예정");
    }
    public void TurnOff_BlueEnergyUsing()
    {

        Debug.Log("블루에너지버튼활성화 오프 UI 추가 예정");
    }
    public void OnDeniedBlueEnergyUsing()
    {
        Debug.Log("블루에너지 부족 버튼 추가 예정");
    }

    public void SetBlueEnergyValue(byte value)
    {
        if(blueEnergyCoroutine != null)
        {
            StopCoroutine(blueEnergyCoroutine);
        }
        blueEnergyCoroutine = StartCoroutine(BlueEnergyCoroutine(value));

        if(value >= 25)
        {
            blueEnergyButton.enabled = true;
        }
        else
        {
            blueEnergyButton.enabled = false;
        }
    }


    private IEnumerator BlueEnergyCoroutine(float value)
    {
        float minValue = 0.3f;

        if (blueEnergySlider.value > value)
        {
            while (blueEnergySlider.value > value)
            {
                float minusValue = (blueEnergySlider.value - value) * 5f
                    * Time.deltaTime;

                if (minValue > minusValue)
                {
                    blueEnergySlider.value -= minValue;
                }
                else
                {
                    blueEnergySlider.value -= minusValue;
                }

                yield return null;
            }
        }
        else
        {
            while (blueEnergySlider.value < value)
            {
                float plusValue = (value - blueEnergySlider.value) * 5f
                    * Time.deltaTime;

                if (minValue > plusValue)
                {
                    blueEnergySlider.value += minValue;
                }
                else
                {
                    blueEnergySlider.value += plusValue;
                }

                yield return null;
            }
        }

        blueEnergySlider.value = value;
        blueEnergyCoroutine = null;
    }

    public void SetOtherPlayers_BlueEnergyValue(byte value)
    {
        if(otherPlayerBlueEnergyCoroutine != null)
        {
            StopCoroutine(otherPlayerBlueEnergyCoroutine);
        }
        otherPlayerBlueEnergyCoroutine =
            StartCoroutine(OtherPlayer_BlueEnergyCoroutine(value));
    }

    private IEnumerator OtherPlayer_BlueEnergyCoroutine(float value)
    {
        float minValue = 0.3f;

        if(otherPlayerBlueEnergySlider.value > value)
        {
            while (otherPlayerBlueEnergySlider.value > value)
            {
                float minusValue = (otherPlayerBlueEnergySlider.value - value) * 5f
                    * Time.deltaTime;

                if(minValue > minusValue)
                {
                    otherPlayerBlueEnergySlider.value -= minValue;
                }
                else
                {
                    otherPlayerBlueEnergySlider.value -= minusValue;
                }

                yield return null;
            }
        }
        else
        {
            while(otherPlayerBlueEnergySlider.value < value)
            {
                float plusValue = (value - otherPlayerBlueEnergySlider.value) * 5f
                    * Time.deltaTime;

                if(minValue > plusValue)
                {
                    otherPlayerBlueEnergySlider.value += minValue;
                }
                else
                {
                    otherPlayerBlueEnergySlider.value += plusValue;
                }

                yield return null;
            }
        }

        otherPlayerBlueEnergySlider.value = value;
        otherPlayerBlueEnergyCoroutine = null;
    }


    public void EnableSiezeFireMode(UInt64 siegingPlayerId , Byte siegingPlayersPivotIndex)
    {
        siegeFireButton.gameObject.SetActive(true);
        siegeFireButton.enabled = false;

        Color color = siegeFireButtonImage.color;
        color.a = 0.3f;
        siegeFireButtonImage.color = color;

        this.siegingPlayerId = siegingPlayerId;
        this.siegingPlayersPivotIndex = siegingPlayersPivotIndex;

        if (NetObjectManager.instance.players[NetworkManager.instance.playerId].attachedPivot
            == (Pivot)NetObjectManager.instance.heightObjects[siegingPlayersPivotIndex])
        {
            siegeFireButton.enabled = true;
            color.a = 1;
            siegeFireButtonImage.color = color;

            Debug.Log("Enable Check");
        }

        NetObjectManager.instance.event_ACQUIRE_ATTACH_TO_PIVOT += On_ACQUIRE_ATTACH_TO_PIVOT;
        NetObjectManager.instance.event_ACQUIRE_JUMP += On_ACQUIRE_JUMP;

    }
    public void DisableSizeFireMode()
    {
        siegeFireButton.gameObject.SetActive(false);

        NetObjectManager.instance.event_ACQUIRE_ATTACH_TO_PIVOT -= On_ACQUIRE_ATTACH_TO_PIVOT;
        NetObjectManager.instance.event_ACQUIRE_JUMP -= On_ACQUIRE_JUMP;

        siegingPlayerId = 0;
    }
    private void On_ACQUIRE_ATTACH_TO_PIVOT(UInt64 attachingPlayerId, byte attachingPivotIndex)
    {
        if(attachingPlayerId == NetworkManager.instance.playerId)
        {
            if ((Pivot)NetObjectManager.instance.heightObjects[siegingPlayersPivotIndex]
                == (Pivot)NetObjectManager.instance.heightObjects[attachingPivotIndex])
            {
                if(siegeFireButton.enabled == false)
                {
                    siegeFireButton.enabled = true;
                    Color color = siegeFireButtonImage.color;
                    color.a = 1;
                    siegeFireButtonImage.color = color;

                    Debug.Log("Enabled Check");
                }
            }
        }

    }
    private void On_ACQUIRE_JUMP(UInt64 jumpingPlayerId)
    {
        if(jumpingPlayerId == NetworkManager.instance.playerId)
        {
            if(siegeFireButton.enabled == true)
            {
                siegeFireButton.enabled = false;
                Color color = siegeFireButtonImage.color;
                color.a = 0.3f;
                siegeFireButtonImage.color = color;

                Debug.Log("Disabled Check");
            }
        }
    }

    private void OnSiegeFireButtonClicked()
    {
        CPacket msg = CPacket.PopForCreate((UInt16)C.C_BLUE_SIZE_MODE_FIRE);
        msg.Push((UInt64)siegingPlayerId); // SiegingPlayerID

        NetworkManager.instance.Send(msg);
    }

}
