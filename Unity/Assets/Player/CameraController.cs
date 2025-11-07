using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public static CameraController instance;
    private Coroutine lookingCoroutine;

    private const float verticalOffset = 4f;
    private const float cameraOrthoSize = 14f;

    private Vector2 desiredPos;

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


        transform.position = transform.position + new Vector3(0, 0, -1f);

        Camera.main.orthographicSize = cameraOrthoSize;

    }

    public void DirectMovePos(Vector2 movePos)
    {
        transform.position = new Vector3(transform.position.x + movePos.x
            , transform.position.y + movePos.y, -1f);
        if(lookingCoroutine != null)
        {
            StopCoroutine(lookingCoroutine);

            desiredPos += movePos;
            lookingCoroutine = StartCoroutine(LookAtCoroutine(desiredPos));
        }
    }

    public void LookAt(Vector2 position)
    {
        if(lookingCoroutine != null)
        {
            StopCoroutine(lookingCoroutine);
        }

        desiredPos = position + new Vector2(0, verticalOffset);
        lookingCoroutine = 
            StartCoroutine(LookAtCoroutine(desiredPos));
    }

    private IEnumerator LookAtCoroutine(Vector2 pos)
    {
        while( Mathf.Pow(transform.position.x - pos.x, 2) + 
            Mathf.Pow(transform.position.y- pos.y, 2) >  0.1f)
        {
            float x = Mathf.Lerp(transform.position.x, pos.x, 5f * Time.deltaTime);
            float y = Mathf.Lerp(transform.position.y, pos.y, 5f * Time.deltaTime);

            transform.position = new Vector3(x, y, -1f);

            yield return null;
        }

        transform.position = new Vector3(pos.x, pos.y, -1f);
        lookingCoroutine = null;
    }
}
