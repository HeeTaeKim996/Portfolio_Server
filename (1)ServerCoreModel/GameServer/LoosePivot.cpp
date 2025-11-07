#include "pch.h"
#include "LoosePivot.h"
#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif
#include "Room.h"

LoosePivot::LoosePivot(UINT64 ownerId, UINT16 index, Vector2 pos, float rot)
	:Pivot(ownerId, index, pos, rot, O_LOOSE_PIVOT)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddLoosePivot();
#endif

}

LoosePivot::~LoosePivot()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveLoosePivot();
#endif
}

HeightObjectRef LoosePivot::Clone() const
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddLoosePivot();
#endif
	return ObjectPool<LoosePivot>::MakeShared_DeepCopy(*this);
}

void LoosePivot::MakeWobble(UINT64 afterDeleteTick)
{
	_isWobble = true;
	_deleteTickTime = GetTickCount64() + afterDeleteTick;
}
