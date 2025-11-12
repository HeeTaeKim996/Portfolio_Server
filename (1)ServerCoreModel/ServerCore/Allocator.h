/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once

#include "Allocator.h"


/*----------------------
	   BaseAllocator
----------------------*/
class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};


/*--------------------------
	    StompAllocator
--------------------------*/
class StompAllocator
{
	enum{PAGE_SIZE = 0x1000};

public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};


/*--------------------------
	    PoolAllocator
--------------------------*/
class PoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};


/*------------------------
        StlAllocator
------------------------*/
template<typename T>
class StlAllocator
{
public:
	using value_type = T;
	StlAllocator() {}
	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}

	inline T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));

		return static_cast<T*>(PoolAllocator::Alloc(size));
	}
	inline void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}
};