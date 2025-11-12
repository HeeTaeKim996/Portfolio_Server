#include "pch.h"
/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#include "Memory.h"
#include "MemoryPool.h"

/*------------------
	   Memory
------------------*/

Memory::Memory()
{
	int32 size = 32;
	int32 tableIndex = 0;


	for (; size < 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size < 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	// DEBUG
	{
		for (int i = 0; i <= 4096; i++)
		{
			ASSERT_CRASH(_poolTable[i] != nullptr);
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
	{
		delete pool;
	}

	_pools.clear();
}

void* Memory::Allocate(int32 size)
{
	MemoryEntry* memory = nullptr;
	const int32 allocSize = size + sizeof(MemoryEntry);

#ifdef _STOMP
	memory = reinterpret_cast<MemoryEntry*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		memory = reinterpret_cast<MemoryEntry*>(_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		memory = _poolTable[allocSize]->pop();
	}
#endif 

	return MemoryEntry::AttachEntry(memory, allocSize);
}

void Memory::Release(void* ptr)
{
	MemoryEntry* memory = MemoryEntry::DetachEntry(ptr);

	const int32 allocSize = memory->allocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	StompAllocator::Release(memory);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		::_aligned_free(memory);
	}
	else
	{
		_poolTable[allocSize]->Push(memory);
	}
#endif 
}
