#pragma once
#include <atomic>
#include "pch.h"

#ifdef _MEMORY_COUNT


class MemoryCount
{
public:
	static void AddClientSession() { _clientSessionMemoryCount.fetch_add(1); }
	static void RemoveClientSession() { _clientSessionMemoryCount.fetch_sub(1); }
	static int32_t ClientSessionCount() { return _clientSessionMemoryCount.load(); }


private:
	inline static std::atomic<int32_t> _clientSessionMemoryCount{ 0 };

public :
	static void AddPlayer() { _playerMemoryCount.fetch_add(1); }
	static void RemovePlayer() { _playerMemoryCount.fetch_sub(1); }
	static int32_t PlayerCount() { return _playerMemoryCount.load(); }

private:
	inline static std::atomic<int32_t> _playerMemoryCount{ 0 };


public:
	static void AddCapsuleRoom() { _capsuleRoomMemoryCount.fetch_add(1); }
	static void RemoveCapsuleRoom() { _capsuleRoomMemoryCount.fetch_sub(1); }
	static int32_t CapsuleRoomCount() { return _capsuleRoomMemoryCount.load(); }

private:
	inline static std::atomic<int32_t> _capsuleRoomMemoryCount{ 0 };


public:
	static void AddMultiRoom() { _multiRoomMemoryCount.fetch_add(1); }
	static void RemoveMultiRoom() { _multiRoomMemoryCount.fetch_sub(1); }
	static int32_t MultiRoomCount() { return _multiRoomMemoryCount.load(); }

private:
	inline static std::atomic<int32_t> _multiRoomMemoryCount{ 0 };


public:
	static void AddBasePivot() { _basePivotMemoryCount.fetch_add(1); }
	static void RemoveBasePivot() { _basePivotMemoryCount.fetch_sub(1); }
	static int32_t BasePivotCount() { return _basePivotMemoryCount.load(); }

private:
	inline static std::atomic<int32_t> _basePivotMemoryCount{ 0 };


public:
	static void AddSlickPivot() { _slickPivotMemoryCount.fetch_add(1); }
	static void RemoveSlickPivot() { _slickPivotMemoryCount.fetch_sub(1); }
	static int32_t SlickPivotCount() { return _slickPivotMemoryCount.load(); }

private:
	inline static std::atomic<int32_t> _slickPivotMemoryCount{ 0 };


public:
	static void AddBlueEnergy(){ _blueEnergyCount.fetch_add(1); }
	static void RemoveBlueEnergy() { _blueEnergyCount.fetch_sub(1); }
	static int32_t BlueEnergyCount() { return _blueEnergyCount.load(); }

private:
	inline static std::atomic<int32_t> _blueEnergyCount{ 0 };


public:
	static void AddLoosePivot() { _loosePivotCount.fetch_add(1); }
	static void RemoveLoosePivot() { _loosePivotCount.fetch_sub(1); }
	static int32_t LoosePivotCount() { return _loosePivotCount.load(); }

private:
	inline static std::atomic<int32_t> _loosePivotCount{ 0 };


public:
	static void AddStrongBird() { _strongBirdCount.fetch_add(1); }
	static void RemoveStrongBird() { _strongBirdCount.fetch_sub(1); }
	static int32_t StrongBirdCount() { return _strongBirdCount.load(); }

private:
	inline static std::atomic<int32_t> _strongBirdCount{ 0 };
};
#endif