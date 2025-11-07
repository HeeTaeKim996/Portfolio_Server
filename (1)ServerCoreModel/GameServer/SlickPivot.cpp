#include "pch.h"
#include "SlickPivot.h"
#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 


SlickPivot::SlickPivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot)
	: Pivot(ownerId, index, pos, rot, O_SLICK_PIVOT)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddSlickPivot();
#endif 
}


SlickPivot::~SlickPivot()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveSlickPivot();
#endif 
}

HeightObjectRef SlickPivot::Clone() const
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddSlickPivot();
#endif

	return ObjectPool<SlickPivot>::MakeShared_DeepCopy(*this);
}
