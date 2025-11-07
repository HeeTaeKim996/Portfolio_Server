/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once

#include <stack>
#include <map>
#include <vector>


/*--------------------------
	   DesdLockProfiler
---------------------------*/
class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);

private:
	void CheckCycle();
	void Dfs(int32 here);

private:
	mutex _lock;

private:
	unordered_map<const char*, int32> _nameToId;
	unordered_map<int32, const char*> _idToName;

	map<int32, set<int32>> _lockHistory;

private:
	vector<int32> _discoveredOrder;
	int32 _discoveredCount;
	vector<bool> _finished;
	vector<int32> _parent;
};



/* ○ 요약
   - Lock-Unlock의 후입선출 구조로 인해,  1 -> 2 -> 1, A -> B -> C -> A 등 싸이클 구조에서 데드락이 확정적으로 발생
   - _nameToId 와 idToName의 int32는 식별자 용도일 뿐, 대소관계는 의미 없음
   - id별 락 해제전에 호출되는 상이한 락을 _lockHistory에 기록하고, 기록시 CheckCycle을 호출하여, 
     _discoveredOrder의 대소관계로 부모관계를 비교, _finished로 Dfs(there)의 종료 유무를 판단하여, 싸이클 유무를 파악
*/