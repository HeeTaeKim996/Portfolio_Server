using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController_PC : PlayerController
{



    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            playerMovement.OnTouchBegan();  
        }    
        else if (Input.GetKey(KeyCode.Space))
        {
            playerMovement.OnTouchStationary(Vector2.zero);
        }
        else if (Input.GetKeyUp(KeyCode.Space))
        {
            playerMovement.OnTouchEnd();
        }
    }
}
