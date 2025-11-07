using System;
using System.Collections;
using System.Collections.Generic;
using CSharpServerCore;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneLoadFinishedInformer : MonoBehaviour
{
    public bool ForDevelop_GameStartRightNow;

    private void Start()
    {
        NetworkManager.instance.OnSceneLoadFinished();


        { // ForDevelop
#if false
            if (ForDevelop_GameStartRightNow && SceneManager.GetActiveScene().name == "Lobby")
            {

                StartCoroutine(ForDevelopRightNOwSTart());

            }
#endif
        }
    }

    private IEnumerator ForDevelopRightNOwSTart()
    {
        yield return new WaitForSeconds(0.2f);

        CPacket msg = CPacket.PopForCreate((UInt16)C.C_MAKE_ROOM);
        NetworkManager.instance.Send(msg);
    }
}
