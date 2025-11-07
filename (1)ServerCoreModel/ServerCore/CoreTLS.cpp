#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#include "CoreTLS.h"

thread_local uint32 LThreadId = 0;

thread_local std::stack<int32> LLockStacks;

thread_local SendBufferChunkRef LSendBufferChunk;

thread_local uint64 LEndTickCount = 0;

thread_local JobQueue* LCurrentJobQueue = nullptr;

