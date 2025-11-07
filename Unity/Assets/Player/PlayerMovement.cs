using System;
using System.Collections;
using System.ComponentModel;
using CSharpServerCore;
using UnityEngine;


public class PlayerMovement : NetObject
{
    private Animator playerAnimator;

    private enum AnimationName : Byte
    {
        Idle,
        Swing,
        Jump,
        BatPose,
    }



    private Rigidbody2D playerRigidbody;
    private PlayerController playerController;
    private CameraController cameraController;
    private PlayersAttachingCollider attachingCollider;
    public float radFromPivot { get; private set; }

    private float pivotsRadius = 2f;

    private Coroutine swingCoroutine;
    private float swingSpeed;
    private bool isSwingClockwise;

    private SpriteRenderer spriteRenderer;

    public Pivot attachedPivot { get; private set; }
    private float primaryGravityScale;


    public PS state { get; private set; } = PS.PS_Jumping;


    protected override void Awake()
    {
        base.Awake();
        playerAnimator = GetComponent<Animator>();
        playerRigidbody = GetComponent<Rigidbody2D>();
        attachingCollider = GetComponentInChildren<PlayersAttachingCollider>();
        spriteRenderer = GetComponent<SpriteRenderer>();
        primaryGravityScale = playerRigidbody.gravityScale;
    }
    public override void Initialize(ulong ownerId)
    {
        base.Initialize(ownerId);
        if (isMine)
        {
            playerController = FindObjectOfType<PlayerControllerManager>().UsingController();
            playerController.SetPlayer(this);
            cameraController = FindObjectOfType<CameraController>();
            GameManager.instance.SetPlayer(this);
        }
        else
        {
            playerRigidbody.gravityScale = 0;
        }
        attachingCollider.SetIsMine(isMine);
    }

    private void Update()
    {
        if (!isMine) return;
        if (state == PS.PS_Idle)
        {
            swingSpeed = 2f;

            if (radFromPivot >= Mathf.PI / -2f && radFromPivot < Mathf.PI / 2f)
            {
                radFromPivot -= swingSpeed * Time.deltaTime;
            }
            else
            {
                radFromPivot += swingSpeed * Time.deltaTime;
            }

            UpdatePositionFromRad();
        }
    }
    private void FixedUpdate()
    {
        if (isMine)
        {
            CPacket msg = CPacket.PopForCreate((UInt16)C.C_PLAYER_UPDATE_ROT_POS);
            Vector2 position = transform.position;

            msg.Push((float)position.x); // posX
            msg.Push((float)position.y); // posY

            msg.Push((float)transform.eulerAngles.z); // rot

            NetworkManager.instance.Send(msg);
        }
    }


    public void SyncRotPos_Others(CPacket msg)
    {
        transform.position = new Vector2(msg.Pop_Float(), msg.Pop_Float());
        transform.rotation = Quaternion.Euler(new Vector3(0, 0, msg.Pop_Float()));
    }

    public void Sync_Flip(CPacket msg)
    {
        Byte isFlip = msg.Pop_Byte();
        if(isFlip == (Byte)1)
        {
            spriteRenderer.flipX = true;
        }
        else
        {
            spriteRenderer.flipX = false;
        }
    }

    public void Require_AttachToPivot(Pivot pivot)
    {
        if (state != PS.PS_Jumping && state != PS.PS_FALLING) return;

        attachingCollider.gameObject.SetActive(false);
        
        playerRigidbody.gravityScale = 0;
        Vector2 dir = transform.position - pivot.transform.position;
        radFromPivot = Mathf.Atan2(dir.y, dir.x);

        attachedPivot = pivot;
        UpdatePositionFromRad();

        cameraController.LookAt((Vector2)pivot.Pos());

        CPacket msg = CPacket.PopForCreate((UInt16)C.C_REQUIRE_ATTACH_TO_PIVOT);
        msg.Push((byte)pivot.Index);

        Debug.Log(pivot.Index.ToString() + " // "  + Time.time);

        NetworkManager.instance.Send(msg);
    }

    public void Acquire_AttachToPivot(Pivot pivot)
    {
        if (!isMine) return;

        state = PS.PS_Idle;
        ChangeAnimation(AnimationName.Idle);
    }


    public void OnTouchBegan()
    {
        if (state == PS.PS_Idle)
        {
            state = PS.PS_Swing;
            ChangeAnimation(AnimationName.Swing);

            if (swingCoroutine != null)
            {
                StopCoroutine(swingCoroutine);
            }

            string pivotTypeName = attachedPivot.GetType().Name;

            if (isSwingClockwise && transform.eulerAngles.y == 0)
            {
                //transform.rotation = transform.rotation * Quaternion.Euler(0, 180f, 0);
            }
            else if (!isSwingClockwise && transform.eulerAngles.y == 180)
            {
                //transform.rotation = transform.rotation * Quaternion.Euler(0, -180f, 0);
            }


            if(radFromPivot >= Mathf.PI / -2f && radFromPivot < Mathf.PI / 2f)
            {
                if (!isSwingClockwise)
                {
                    isSwingClockwise = true;
                    spriteRenderer.flipX = true;

                    CPacket msg = CPacket.PopForCreate((UInt16)C.C_PLAYER_FLIP);
                    msg.Push((Byte)1);
                    NetworkManager.instance.Send(msg);
                }
            }
            else
            {
                if (isSwingClockwise)
                {
                    isSwingClockwise = false;
                    spriteRenderer.flipX = false;

                    CPacket msg = CPacket.PopForCreate((UInt16)C.C_PLAYER_FLIP);
                    msg.Push((Byte)0);
                    NetworkManager.instance.Send(msg);
                }
            }


            switch (pivotTypeName) 
            {
                case "BasePivot":
                    {
                        swingCoroutine = StartCoroutine(BaseSwingCoroutine());
                    }
                    break;
                case "SlickPivot":
                    {
                        swingCoroutine = StartCoroutine(SlickSwingCoroutine());
                    }
                    break;
                case "LoosePivot":
                    {
                        swingCoroutine = StartCoroutine(BaseSwingCoroutine());
                    }
                    break;
                case "StrongBirdPivot":
                    {
                        swingCoroutine = StartCoroutine(BaseSwingCoroutine());
                    }
                    break;
                default:
                    {
                        Debug.LogError("NOT_ASSIGNED_PIVOT");
                    }
                    break;
            }

            CPacket msg2 = CPacket.PopForCreate((UInt16)C.C_ANNOUNCE_SWING);
            NetworkManager.instance.Send(msg2);
        }
    }
    public void OnTouchStationary(Vector2 touchPos)
    {
        if(state == PS.PS_BlueAiming && attachedPivot != null)
        {
            Vector3 pivotPos = attachedPivot.transform.position;
            Vector3 worldPosition = Camera.main.ScreenToWorldPoint(new Vector3(touchPos.x, touchPos.y, 0));

            Vector3 dir = pivotPos - worldPosition;

            radFromPivot = Mathf.Atan2(dir.y, dir.x);
            UpdatePositionFromRad();
        }


    }
    public void OnTouchEnd()
    {
        if (state == PS.PS_Swing)
        {
            if (swingCoroutine != null)
            {
                StopCoroutine(swingCoroutine);
                swingCoroutine = null;
            }

#if _Develop
            // For Develop Modes
            if (GameManager.instance.isCantFailMode)
            {
                CPacket msg2 = CPacket.PopForCreate((UInt16)C.C_ANNOUCE_JUMP);
                NetworkManager.instance.Send(msg2);

                playerRigidbody.gravityScale = primaryGravityScale;
                playerRigidbody.velocity = Vector2.zero;


                Collider2D[] colliders = Physics2D.OverlapCircleAll(transform.position, 20f, attachingCollider.pivotLayer);

                state = PS.PS_Jumping;

                if (colliders.Length > 0)
                {
                    float lowestHeightDiff = float.MaxValue;
                    int lowestIndex = -1;

                    for (int i = 0; i < colliders.Length; i++)
                    {
                        float diffFloat = colliders[i].transform.position.y - attachedPivot.transform.position.y;
                        if (diffFloat < lowestHeightDiff && diffFloat > 0)
                        {
                            lowestHeightDiff = diffFloat;
                            lowestIndex = i;
                        }
                    }

                    if(lowestIndex == -1)
                    {
                        Debug.LogError("LOWEST INDEX IS -1");
                    }
                    else
                    {
                        transform.position = colliders[lowestIndex].transform.position;
                    }
                }
                else
                {
                    Debug.Log("No Pivot For Jump");
                }

                attachingCollider.gameObject.SetActive(true);

                return;
            }
#endif
            Jump();

        }
        else if(state == PS.PS_BlueAiming)
        {
            if(radFromPivot > - MathF.PI && radFromPivot < 0)
            {
                CPacket msg = CPacket.PopForCreate((UInt16)C.C_BLUE_SIEZE_MODE);
                NetworkManager.instance.Send(msg);
            }
            else
            {
                if(GameManager.instance.onGameUIPanel.blueEnergySlider.value >= 50)
                {
                    CPacket msg = CPacket.PopForCreate((UInt16)C.C_REQUIRE_BLUE_SELF_RACKET);
                    msg.Push((float)transform.position.x); // posX
                    msg.Push((float)transform.position.y); // posY
                    msg.Push((float)radFromPivot); // Rad From Pivot

                    NetworkManager.instance.Send(msg);
                }
                else
                {
                    // �Ʒ� �� �ڵ��, �������� �˼��ؼ� �������ݷ� �ͼ� ����ǰų�,
                    // ��ó�� Ŭ�� ��ü �˼� ���� 2��θ� ���� ���� ����
                    GameManager.instance.onGameUIPanel.OnDeniedBlueEnergyUsing();
                    Handle_DENIED_BLUE_RACKET();
                }
            }
        }
    }

    private void Jump(bool isFalling = false)
    {
        if (isFalling)
        {
            state = PS.PS_FALLING;
        }
        else
        {
            state = PS.PS_Jumping;
            ChangeAnimation(AnimationName.Jump);
        }

        attachedPivot = null;
        playerRigidbody.gravityScale = primaryGravityScale;
        playerRigidbody.velocity = Vector2.zero;
        StartCoroutine(AttachingColliderOnCoroutine());

        float directionRad;
        if (!isSwingClockwise)
        {
            directionRad = radFromPivot + Mathf.PI / 2.3f;
        }
        else
        {
            directionRad = radFromPivot - Mathf.PI / 2.3f;
        }

        Vector2 direction = new Vector2(Mathf.Cos(directionRad), Mathf.Sin(directionRad));
        playerRigidbody.AddForce(direction.normalized * (10f + swingSpeed * 130f));

        CPacket msg = CPacket.PopForCreate((UInt16)C.C_ANNOUCE_JUMP);
        NetworkManager.instance.Send(msg);


    }


    public void CancelJump()
    {
        if(state == PS.PS_Swing)
        {
            if (swingCoroutine != null)
            {
                StopCoroutine(swingCoroutine);
                swingCoroutine = null;
            }
            state = PS.PS_Idle;
            ChangeAnimation(AnimationName.Idle);

            CPacket msg = CPacket.PopForCreate((UInt16)C.C_ANNOUNCE_IDLE);
            NetworkManager.instance.Send(msg);
        }
    }


    private IEnumerator AttachingColliderOnCoroutine(float waitTime = 0.15f)
    {
        yield return new WaitForSeconds(waitTime);

        attachingCollider.gameObject.SetActive(true);
    }

    private IEnumerator BaseSwingCoroutine()
    {
        swingSpeed = 5f;
        float maxSwingSpeed = 9f;

        while (true)
        {
            if(swingSpeed > maxSwingSpeed)
            {
                swingSpeed = maxSwingSpeed;
            }
            else
            {
                swingSpeed += 1.4f * Time.deltaTime;
            }


            if (isSwingClockwise)
            {
                radFromPivot -= swingSpeed * Time.deltaTime;
                if (radFromPivot < -Mathf.PI)
                {
                    radFromPivot += Mathf.PI * 2f;
                }
            }
            else
            {
                radFromPivot += swingSpeed * Time.deltaTime;
                if (radFromPivot > Mathf.PI)
                {
                    radFromPivot -= Mathf.PI * 2f;
                }
            }
            UpdatePositionFromRad();

            yield return null;
        }
    }
    private IEnumerator SlickSwingCoroutine()
    {
        swingSpeed = 6.5f;
        float maxSwingSpeed = 12f;

        while (true)
        {
            if (swingSpeed > maxSwingSpeed)
            {
                swingSpeed = maxSwingSpeed;
            }
            else
            {
                swingSpeed += 1.6f * Time.deltaTime;
            }


            if (isSwingClockwise)
            {
                radFromPivot -= swingSpeed * Time.deltaTime;
                if (radFromPivot < -Mathf.PI)
                {
                    radFromPivot += Mathf.PI * 2f;
                }
            }
            else
            {
                radFromPivot += swingSpeed * Time.deltaTime;
                if (radFromPivot > Mathf.PI)
                {
                    radFromPivot -= Mathf.PI * 2f;
                }
            }
            UpdatePositionFromRad();

            yield return null;
        }

    }

    private void UpdatePositionFromRad()
    {
        if(attachedPivot != null)
        {
            float x = attachedPivot.transform.position.x + Mathf.Cos(radFromPivot) * pivotsRadius;
            float y = attachedPivot.transform.position.y + Mathf.Sin(radFromPivot) * pivotsRadius;
            transform.position = new Vector2(x, y);

            float interpolRad;
            if(state == PS.PS_Swing)
            {
                interpolRad = isSwingClockwise ? 120f : +60f;
            }
            else if(state == PS.PS_BlueAiming || state == PS.PS_Idle)
            {
                interpolRad = 90;
            }
            else
            {
                interpolRad = 0;
            }

            transform.rotation = Quaternion.Euler(new Vector3(0, 0,
                radFromPivot * 180f / (float)Math.PI + interpolRad));
        }
    }

    public void OnEnergyLayerCollide(Energy energy)
    {
        CPacket msg = CPacket.PopForCreate((UInt16)C.C_REQUIRE_ENERGY);
        msg.Push((Byte)energy.Index);

        NetworkManager.instance.Send(msg);
    }

    public void OnBlueEnergyButtonClicked()
    {
        if(state == PS.PS_Idle)
        {
            GameManager.instance.onGameUIPanel.OnBlueEnergyUsing();

            playerRigidbody.gravityScale = 0;
            playerRigidbody.velocity = Vector2.zero;

            state = PS.PS_BlueAiming;
            ChangeAnimation(AnimationName.BatPose);

            UpdatePositionFromRad();
        }
        else if(state == PS.PS_BlueAiming)
        {
            GameManager.instance.onGameUIPanel.TurnOff_BlueEnergyUsing();
            state = PS.PS_Idle;
        }
        else if(state == PS.PS_BLUE_SIEGE_MODE)
        {
            CPacket msg = CPacket.PopForCreate((UInt16)C.C_REQUIRE_CANCEL_SIEZE_MODE);
            NetworkManager.instance.Send(msg);
        }
    }

    public void Handle_ACQUIRE_BLUE_SELF_RACKET(CPacket msg)
    {
        ChangeAnimation(AnimationName.Jump);
        StartCoroutine(AttachingColliderOnCoroutine());

        state = PS.PS_Jumping;
        playerRigidbody.gravityScale = primaryGravityScale;
        playerRigidbody.velocity = Vector2.zero;

        transform.position = new Vector2(msg.Pop_Float(), msg.Pop_Float());

        float fromServer_RadFromPivot = msg.Pop_Float();
        Vector2 direction = new Vector2(Mathf.Cos(fromServer_RadFromPivot), 
            Mathf.Sin(fromServer_RadFromPivot));
        playerRigidbody.AddForce(direction.normalized * 1400f);
    }

    public void Handle_DENIED_BLUE_RACKET()
    {
        // ���� ���� �ִϸ��̼� �߰��� ������
        ChangeAnimation(AnimationName.Swing);


        state = PS.PS_Idle;
    }

    private void ChangeAnimation(AnimationName animationName)
    {
        playerAnimator.Play(animationName.ToString());

        CPacket msg = CPacket.PopForCreate((UInt16)C.C_SYNC_PLAYER_ANIMATION);
        msg.Push((Byte)animationName); // AnimatioName

        NetworkManager.instance.Send(msg);
    }

    public void SyncAnimation(CPacket msg)
    {
        playerAnimator.Play(((AnimationName)msg.Pop_Byte()).ToString());
    }

    public void Acquire_BlueSiezeMode()
    {
        state = PS.PS_BLUE_SIEGE_MODE;
    }
    public void Acquire_Cancel_SiezeMode()
    {
        state = PS.PS_Idle;
        ChangeAnimation(AnimationName.Idle);
    }

    public void SiegeModeFiring_Sieger()
    {
        state = PS.PS_BLUE_SIEGE_FIRING;
        // ���� �ִϸ��̼� �߰�
    }
    public void SiegeModeFiring_Flyer(Vector2 aimPos)
    {
        state = PS.PS_BLUE_SIEGE_FIRING;
        // ���� �ִϸ��̼� �߰�

        if(swingCoroutine != null)
        {
            StopCoroutine(swingCoroutine);
        }
        swingCoroutine = StartCoroutine(MoveToSiegerPosCoroutine(aimPos));
    }
    private IEnumerator MoveToSiegerPosCoroutine(Vector2 aimPos)
    {
        float duration = 1f;
        float elapsedTime = 0;
        Vector2 initialPos = transform.position;

        while (elapsedTime < duration)
        {
            elapsedTime += Time.deltaTime;

            transform.position = Vector2.Lerp(initialPos, aimPos,
                elapsedTime / duration);

            yield return null;
        }

        transform.position = aimPos;
        swingCoroutine = null;
    }

    public void SiegeModeFire_Sieger()
    {
        state = PS.PS_Idle;
        ChangeAnimation(AnimationName.Swing); // ���� ��� �ٸ� �ɷ� �߰� ����
    }

    public void SiegeModeFire_Flyer(Vector2 direction)
    {
        if(swingCoroutine != null)
        {
            StopCoroutine(swingCoroutine);
            swingCoroutine = null;
        }

        ChangeAnimation(AnimationName.Jump);
        StartCoroutine(AttachingColliderOnCoroutine(0.3f));

        state = PS.PS_Jumping;
        playerRigidbody.gravityScale = primaryGravityScale;
        playerRigidbody.velocity = Vector2.zero;

        playerRigidbody.AddForce(direction.normalized * 1400f);
    }

    public void Drop(bool isFalling)
    {
        Jump(isFalling);
    }
}
