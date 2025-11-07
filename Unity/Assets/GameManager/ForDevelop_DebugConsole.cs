using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.UI;

public class ForDevelop_DebugConsole : MonoBehaviour
{
#if _Develop
    public Text logText;
    private List<string> logStrings = new List<string>();

    private void Awake()
    {
        Application.logMessageReceived += OnLogMessageReceived;
    }
    public void OnLogMessageReceived(string logString, string stackTrace, LogType type)
    {
        //return;

        logStrings.Add(logString);
        DeleteAfterTime(logString);
        
        if(logStrings.Count > 10)
        {
            logStrings.RemoveAt(0);
        }

        logText.text = string.Join('\n', logStrings.ToArray());
    }

    private IEnumerator DeleteAfterTime(string newLog)
    {
        yield return new WaitForSeconds(5f);

        if (logStrings.Contains(newLog))
        {
            logStrings.Remove(newLog);

            logText.text = string.Join('\n', logStrings.ToArray());
        }

    }

    private void OnDestroy()
    {
        Application.logMessageReceived -= OnLogMessageReceived;
    }


#endif
}
