#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/
#include "DeadLockProfiler.h"

/*--------------------------
	   DesdLockProfiler
---------------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard lockGuard(_lock);

	int32 lockId = 0;

	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else
	{
		lockId = _nameToId[name];
	}

	// 잡고 있는 락이 있다면
	if (!LLockStacks.empty())
	{
		int32 preId = LLockStacks.top();
		if (lockId != preId)
		{
			set<int32>& history = _lockHistory[preId];
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId); // 락을 풀기 전에, 추가로 잡히는, 인접한, 다음방향으로의 상이한 락 id를 기록
				CheckCycle();
				// ※ Lock - Unlock이 후입선출을 전제로 하기에, 이 상태에서 락은 A -> B -> C -> D(추가) 로 꼬리를 무는 구조가 된다
			}
		}
	}

	LLockStacks.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard lockGuard(_lock);

	if (LLockStacks.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	int32 lockId = _nameToId[name];
	if (LLockStacks.top() != lockId) // ※ Lock - UnLock의 후입선출을 전제
	{
		CRASH("INVALID_UNLOCK");
	}

	LLockStacks.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());

	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int i = 0; i < lockCount; i++)
	{
		Dfs(i);
	}

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] == -1) return;

	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한, 다음 방향으로의 정점 유무 파악
	auto findIt = _lockHistory.find(here);

	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	// 모든 정점을 순회
	set<int32>& history = findIt->second;
	for (int32 there : history)
	{
		// 아직 방문한 적이 없다면, 방문
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here 가 there보다 먼저 발견되었다면, there는 here의 후손. (순방향/간선)
		if (_discoveredOrder[here] <= _discoveredOrder[there])
		{
			continue;
		}

		// 순방향이 아니며, Dfs(there)가 아직 종료되지 않았다면, 위 Lock - Unlock의 후입선출 구조에 의해,
		// 확정적으로 싸이클 구조의 데드락 발생
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);
			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];

				if (now == there)
				{
					break;
				}
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}
	
	_finished[here] = true;
}
