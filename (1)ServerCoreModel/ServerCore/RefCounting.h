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

	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }

	// 이동
	TSharedPtr(TSharedPtr&& rhs) noexcept { _ptr = rhs._ptr; rhs._ptr = nullptr; }

	// 자식 상속 가능
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	template<typename> friend class TSharedPtr;

	~TSharedPtr() { Release(); }

public:
	// 복사
	inline TSharedPtr& operator = (const TSharedPtr& rhs) // ※ A = B = C.. 처럼 사용 대비 리턴값을 TSharedPtr&
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// 이동
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
	/* ○ operator T*
	   - operator T* 는 operator의 특수한 문법으로,
		 암시적으로 T* 로 변환할 수 있는 함수.
	   - 예를 들어, TSharedPtr<A> a ( new A() ); A b = a; b->.... (a의 인스턴스 함수 호출 가능)
	*/


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