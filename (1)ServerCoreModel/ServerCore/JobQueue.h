/* -------------------------------------
	  (1)ServerCoreModel_250528
--------------------------------------*/
#pragma once

#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

/*--------------
	JobQueue
--------------*/
class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	inline void DoAsync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	inline void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
	}

	inline void DoTimer(uint64 tickAfter, CallbackType&& callback)
	{


		JobRef job = ObjectPool<Job>::MakeShared(std::move(callback));

		GJobTimer->Reserve(tickAfter, shared_from_this(), job);


	}

	template<typename T, typename Ret, typename... Args>
	inline void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{

		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());		

		JobRef job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);		

		GJobTimer->Reserve(tickAfter, shared_from_this(), job);


	}


	void ClearJobs() { _jobs.Clear(); }

public:
	void Push(JobRef&& job, bool pushOnly = false);
	void Execute();

private:
	LockQueue<JobRef> _jobs;
	Atomic<int32> _jobCount = 0;
};