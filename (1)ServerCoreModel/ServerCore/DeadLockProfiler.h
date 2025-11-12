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