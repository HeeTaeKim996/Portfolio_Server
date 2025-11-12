/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once
#include <atomic>


enum
{
	SLIST_ALIGNMENT = 16
};

/*---------------------
	  MemoryEntry
---------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryEntry : public SLIST_ENTRY
{
	MemoryEntry(int32 size) : allocSize(size) {}

	inline static void* AttachEntry(MemoryEntry* header, int32 size)
	{
		new(header)MemoryEntry(size);
		return reinterpret_cast<void*>(++header);
	}

	inline static MemoryEntry* DetachEntry(void* ptr)
	{
		MemoryEntry* memory = reinterpret_cast<MemoryEntry*>(ptr) - 1; 
		return memory;
	}

	int32 allocSize;
};

/*--------------------
	  MemoryPool
--------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryEntry* ptr);
	MemoryEntry* pop();

private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	atomic<int32> _useCount = 0;
	atomic<int32> _reservedCount = 0;
};