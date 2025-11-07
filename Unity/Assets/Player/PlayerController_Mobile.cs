using System.Collections;
using System.Collections.Generic;
using System.ComponentModel.Design;
using UnityEngine;

public class PlayerController_Mobile : PlayerController
{

    private int mainTouchId = -1;

    public RectTransform[] settingRects;

#if _Develop
    public GameObject settingRects_ParentObject;
#endif
    private int settingId = -1;


    public GameObject cancelButton;
    private RectTransform cancelRect;
    private float cancelRadius;

    private float screenYMin;
    private float screenXMin;
    private float screenYMax;
    private float screenXMax;

    public RectTransform invalidRect;

    protected override void Awake()
    {
        base.Awake();
        cancelRect = cancelButton.GetComponent<RectTransform>();
        cancelRadius = cancelRect.rect.width / 2f;

        screenYMin = 0 + cancelRadius * 2f;
        screenYMax = Screen.height - cancelRadius * 2f;
        screenXMin = 0 + cancelRadius * 2f;
        screenXMax = Screen.width - cancelRadius;
    }
    protected override void Start()
    {
        base.Start();
#if _Develop
        settingRects_ParentObject.SetActive(false);
#endif
        cancelButton.gameObject.SetActive(false);
    }

    private void Update()
    {
        for(int i = 0; i < Input.touchCount; i++)
        {
            Touch touch = Input.GetTouch(i);
            

            if( (mainTouchId == -1 || mainTouchId == touch.fingerId) && isPlayMode)
            {
                if (touch.phase == TouchPhase.Began && true && 
                    (!IsTouchWithinRect(settingRects[0], touch.position) 
                    && !IsTouchWithinRect(invalidRect, touch.position)))
                {
                    mainTouchId = touch.fingerId;
                    playerMovement.OnTouchBegan();

                    if(playerMovement.state == PS.PS_Swing)
                    {
                        cancelButton.gameObject.SetActive(true);

                        Vector2 cancelPos = touch.position + new Vector2(-250f, -250f);

                        if (cancelPos.y < screenYMin)
                        {
                            cancelPos.y = screenYMin;
                        }
                        if (cancelPos.x < screenXMin)
                        {
                            cancelPos.x = screenXMin;
                        }

                        cancelRect.anchoredPosition = cancelPos;
                    }
                }
                else if( (touch.phase == TouchPhase.Moved || touch.phase == TouchPhase.Stationary) && mainTouchId != -1)
                {
                    playerMovement.OnTouchStationary(touch.position);
                }
                else if( (touch.phase == TouchPhase.Ended || touch.phase == TouchPhase.Canceled) && mainTouchId != -1)
                {
                    if (IsTouchWithinCircle(cancelRect, touch.position, cancelRadius))
                    {
                        playerMovement.CancelJump();
                    }
                    else
                    {
                        playerMovement.OnTouchEnd();
                    }

                    cancelButton.gameObject.SetActive(false);

                    mainTouchId = -1;
                }
            }

            if(settingId == -1 || settingId == touch.fingerId)
            {
                if(touch.phase == TouchPhase.Began && IsTouchWithinRect(settingRects[0], touch.position))
                {
                    settingId = touch.fingerId;
#if _Develop
                    settingRects_ParentObject.SetActive(true);
#endif
                }
                else if( (touch.phase == TouchPhase.Ended || touch.phase == TouchPhase.Canceled) && settingId != -1)
                {
                    if (IsTouchWithinRect(settingRects[0], touch.position))
                    {
                        if (isPlayMode)
                        {
                            settingPanel.OpenSettingPanel();
                            isPlayMode = false;
                        }
                    }

#if _Develop
                    else if (IsTouchWithinRect(settingRects[1], touch.position))
                    {
                        if (GameManager.instance.isCantFailMode)
                        {
                            GameManager.instance.SetCantFailMode(false);
                        }
                        else
                        {
                            GameManager.instance.SetCantFailMode(true);
                        }
                    }
#endif

                    settingId = -1;
#if _Develop
                    settingRects_ParentObject.SetActive(false);
#endif
                }
            }
        }
    }



    private bool IsTouchWithinRect(RectTransform rect, Vector2 touchPos)
    {
        Vector2 localPos;
        if(RectTransformUtility.ScreenPointToLocalPointInRectangle(rect, touchPos, null, out localPos))
        {
            return rect.rect.Contains(localPos);
        }
        return false;
    }
    private bool IsTouchWithinCircle(RectTransform rect, Vector2 touchPos, float radius)
    {
        Vector2 localPos;
        if(RectTransformUtility.ScreenPointToLocalPointInRectangle(rect, touchPos, null, out localPos))
        {
            return localPos.magnitude <= radius;
        }
        return false;
    }
}
