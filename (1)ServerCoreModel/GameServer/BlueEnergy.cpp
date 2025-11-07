#include "pch.h"
#include "BlueEnergy.h"
#ifdef _MEMORY_COUNT
#include "MemoryCount.h"
#endif 


BlueEnergy::BlueEnergy(UINT64 ownerId, Vector2 pos, float rot, UINT16 objectCode, UINT8 index, UINT8 amount)
	: Energy(ownerId, pos, rot, objectCode, index, amount)
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddBlueEnergy();
#endif 

	_energyType = EnergyType::BlueEnergy;
}

BlueEnergy::~BlueEnergy()
{
#ifdef _MEMORY_COUNT
	MemoryCount::RemoveBlueEnergy();
#endif
}

HeightObjectRef BlueEnergy::Clone() const
{
#ifdef _MEMORY_COUNT
	MemoryCount::AddBlueEnergy();
#endif 
	return ObjectPool<BlueEnergy>::MakeShared_DeepCopy(*this);
}
