#include "pch.h"
#include "GlobalQueue.h"
/* -------------------------------------
	  (1)ServerCoreModel_250528
--------------------------------------*/


/*---------------------
	   GlobalQueue
----------------------*/

void GlobalQueue::Push(JobQueueRef jobQueue)
{
	_jobQueues.Push(jobQueue); // WRITE_LOCK
}

JobQueueRef GlobalQueue::Pop()
{
	return _jobQueues.Pop(); // WRITE_LOCK
}
