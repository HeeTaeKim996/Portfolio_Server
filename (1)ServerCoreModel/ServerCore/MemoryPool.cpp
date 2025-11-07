#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#include "MemoryPool.h"

/*--------------------
	  MemoryPool
--------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
	::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	while (MemoryEntry* memory = static_cast<MemoryEntry*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryEntry* ptr)
{
	ptr->allocSize = 0; // allocSize = 0으로 할당함으로써, 더이상 이 헤더가 사용되지 않음을 명시

	// Pool에 메모리 반납
	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));
	// ※ InterlockedPushEntrySList, InterlockedPopEntrySList 는 커널레벨에서 풀링을 관리하는 함수

	_useCount.fetch_sub(1);
	_reservedCount.fetch_add(1);
}

MemoryEntry* MemoryPool::pop()
{
	MemoryEntry* memory = static_cast<MemoryEntry*>(InterlockedPopEntrySList(&_header));

	// 없으면 새로 만든다
	if(!memory)
	{
		memory = reinterpret_cast<MemoryEntry*>(::_aligned_malloc(_allocSize, SLIST_ALIGNMENT));
		// ※ memory 생성자 초기화는, pool 클래스에서 메모리를 pop으로 뽑아 쓸 때 처리한다
	}
	else
	{
		ASSERT_CRASH(memory->allocSize == 0);
		_reservedCount.fetch_sub(1);
	}

	_useCount.fetch_add(1);

	return memory;
}
