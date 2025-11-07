/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	inline static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryEntry* ptr = reinterpret_cast<MemoryEntry*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = reinterpret_cast<Type*>(MemoryEntry::AttachEntry(ptr, s_allocSize));
#else
		Type* memory = reinterpret_cast<Type*>(MemoryEntry::AttachEntry(s_pool.pop(), s_allocSize));
#endif 
		new(memory)Type(forward<Args>(args)...);
		return memory;
	}

	inline static void Push(Type* obj)
	{
		obj->~Type();
		
#ifdef _STOMP
		StompAllocator::Release(MemoryEntry::DetachEntry(obj));
#else
		s_pool.Push(MemoryEntry::DetachEntry(obj));
#endif 
	}

	template<typename... Args>
	inline static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}
	



	inline static Type* Pop_DeepCopy(const Type& original)
	{
#ifdef _STOMP
		MemoryEntry* ptr = reinterpret_cast<MemoryEntry*>(StompAllocator::Alloc(s_allocSize));
		Type* memory = reinterpret_cast<Type*>(MemoryEntry::AttachEntry(ptr, s_allocSize));
#else
		Type* memory = reinterpret_cast<Type*>(MemoryEntry::AttachEntry(s_pool.pop(), s_allocSize));
#endif 
		new(memory) Type(original);
		return memory;
	}
	inline static shared_ptr<Type> MakeShared_DeepCopy(const Type& original)
	{
		shared_ptr<Type> ptr = { Pop_DeepCopy(original), Push };
		return ptr;
	}







private:
	static int32 s_allocSize;
	static MemoryPool s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryEntry);

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };