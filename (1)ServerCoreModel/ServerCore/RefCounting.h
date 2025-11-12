/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once

/*------------------------------
	    TSharedRefferable
------------------------------*/
class TSharedRefferable
{
public:
	TSharedRefferable() {}
	virtual ~TSharedRefferable() {}

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	inline int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount <= 0)
		{
			xdelete(this);
		}
		return refCount;
	}

private:
	atomic<int32> _refCount = 0;
};

/*---------------------
	   TSharedPtr
---------------------*/
template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }

	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }

	TSharedPtr(TSharedPtr&& rhs) noexcept { _ptr = rhs._ptr; rhs._ptr = nullptr; }

	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	template<typename> friend class TSharedPtr;

	~TSharedPtr() { Release(); }

public:
	inline TSharedPtr& operator = (const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	inline TSharedPtr& operator = (TSharedPtr&& rhs) noexcept
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool		operator	==	(const TSharedPtr& rhs) const	{ return _ptr == rhs._ptr; }
	bool		operator	==	(T* ptr)				const	{ return _ptr == ptr; }
	bool		operator	!=	(const TSharedPtr& rhs) const	{ return _ptr != rhs._ptr; }
	bool		operator	!=	(T* ptr)				const	{ return _ptr != ptr; }
	bool		operator	<	(const TSharedPtr& rhs) const	{ return _ptr < rhs._ptr; }
	bool		operator	>	(const TSharedPtr& rhs) const	{ return _ptr > rhs._ptr; }
	
	T*			operator	*	()								{ return _ptr; }
	const T*	operator	*	()						const	{ return _ptr; }


	T*			operator	->	()								{ return _ptr; }
	const T*	operator	->	()						const	{ return _ptr; }


	operator T* () const { return _ptr; }

	bool IsNull() const { return !_ptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
		{
			ptr->AddRef();
		}
	}
	inline void Release()
	{
		if (_ptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};