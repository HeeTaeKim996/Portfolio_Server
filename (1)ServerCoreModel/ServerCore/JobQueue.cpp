#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250528
--------------------------------------*/
#include "JobQueue.h"
#include "GlobalQueue.h"

/*--------------
	JobQueue
--------------*/

void JobQueue::Push(JobRef&& job, bool pushOnly)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK;

	if (prevCount == 0)
	{
		if (pushOnly == false && LCurrentJobQueue == nullptr)
		{
			Execute();
		}
		else
		{
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;
	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs); // WRITE_LOCK

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs[i]->Execute();
		}

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now > LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			GGlobalQueue->Push(shared_from_this());
			return;
		}
	}
}
