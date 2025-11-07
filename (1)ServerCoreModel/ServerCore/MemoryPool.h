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
		return reinterpret_cast<void*>(++header); // ※ ++는 정수 1증가가 아닌, sizeof(MemoryEntry)만큼 증가
	}

	inline static MemoryEntry* DetachEntry(void* ptr)
	{
		MemoryEntry* memory = reinterpret_cast<MemoryEntry*>(ptr) - 1; 
		// ※ 마찬가지로, -1은 정수 1감소가 아닌, sizeof(MemoryEntry) 만큼 감소
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