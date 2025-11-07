/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once
#include "Allocator.h"

class MemoryPool;

/*------------------
	   Memory
------------------*/
class Memory
{
	enum
	{
		// ~1024까지 32씩 사이즈증가, ~2048까지 128씩 사이즈 증가, ~4096까지 256씩 사이즈 증가
		POOL_COUNT = (1024 / 32) + (2048 / 128) + (4096 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<MemoryPool*> _pools;
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1]; 
	// ※ 0 ~ 4096(MAX_ALLOC_SIZE) 까지 필요하므로, 배열의 요소의 수는 4096 + 1
};


template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	// 생성될 객체의 메모리주소를 할당
	Type* memory = reinterpret_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// 할당된 주소에, Type 객체를 생성
	new(memory)Type(forward<Args>(args)...);

	return memory;
}


template<typename Type>
void xdelete(Type* obj)
{
	// 소멸자 호출
	obj->~Type();

	// 할당됐던 주소 반납
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type>{xnew<Type>(forward<Args>(args)...), xdelete<Type>};
}