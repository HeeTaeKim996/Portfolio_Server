/* -------------------------------------
	  (1)ServerCoreModel_250525
--------------------------------------*/
#pragma once

template<typename T>
class LockQueue
{
public:
	inline void Push(T item)
	{
		WRITE_LOCK;
		_items.push(item);
	}

	inline T Pop()
	{
		WRITE_LOCK;
		if (_items.empty())
		{
			return T();
		}
		T ret = _items.front();
		_items.pop();
		return ret;
	}

	inline void PopAll(OUT Vector<T>& items)
	{
		WRITE_LOCK;
		while (T item = Pop())
		{
			items.push_back(item);
		}
	}

	inline void Clear()
	{
		WRITE_LOCK;
		_items = queue<T>();
	}


private:
	USE_LOCK;
	queue<T> _items;
};