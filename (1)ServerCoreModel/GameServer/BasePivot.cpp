#include "pch.h"
#include "BasePivot.h"


#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 


BasePivot::BasePivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot)
	: Pivot(ownerId, index, pos, rot, O_BASE_PIVOT)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddBasePivot();
#endif 
}

BasePivot::~BasePivot()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveBasePivot();
#endif
}

HeightObjectRef BasePivot::Clone() const
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddBasePivot();
#endif 
	return ObjectPool<BasePivot>::MakeShared_DeepCopy(*this);
}

