/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/
#pragma once

/*-----------------
	 RW SpinLock
-----------------*/
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10'000,
		MAX_SPIN_COUNT = 5'000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	uint16 _writeCount = 0;
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
};

/*------------------------
		 LockGuards
------------------------*/
class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) :_lock(lock), _name(name) { lock.WriteLock(name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};
