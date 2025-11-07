/* -------------------------------------
	  (1)ServerCoreModel_250522
--------------------------------------*/

#pragma once

#define OUT

/*---------------
	  Lock
---------------*/
#define	USE_MAY_LOCKS(count)		Lock _locks[count] // 배열 생성
#define USE_LOCK					USE_MAY_LOCKS(1)

#define READ_LOCK_IDX(idx)			ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name())
	/*	_##idx , typeid(this).name()
	   ○ _##idx
	    - 컴파일 때 자동 변환. idx ==2 라면, ReadLockGuard readLockGuard_2(_locks[2], ... 

	   ○ typeid(this).name()
	    - 해당 매크로를 사용하는 클래스명이 전달. Ex). A클래스에서 호출시, typeid(this).name() == 'class A' )
		                                            B구조체에서 호출시, typeid(This).name() == 'struct B' )
		- typeid(this).name() 은 DeadLockProfiler를 위해 사용된는데, DeadLockProfiler가 하나의 클래스에서 하나의 락을 사용한다는
		  전제로 만들어진 듯함

		- @@ 한클래스에서 2개의 락을 사용한다 해도, 데드락프로파일러는 같은 락으로 인식하면 문제가 되는데, 이를 구분할 방법이 있을까
		  -> RadLockGuard readLockGuard_##idx(_locks[idx], (std::string(typeid(this).name()) + std::to_string(idx).c_str())
		     이렇게 만들어도 될 듯 싶다.
			 + 그런데 이렇게 하면, ReadLock을 할때마다 연산 비용이 늘어난다. 
			   위 (2)항 string을 사용하는 곳이, DeadLockProfiler이고, DeadLockProfiler는 디버그 모드에서만 작동하니,
			   이 클래스 코어매크로에서, #ifdef로, 디버그일때는 위처럼, 릴리즈일때에는 string을 nullptr로 주는 것도 괜찮을듯
		   ==>> @@ 내용은, (1)ServerCoreModel 에서 하지 말고, 우선 수정본들이 다 작동하는 걸 확인하고, 그 다음 수정 때 처리하자
	*/
#define READ_LOCK					READ_LOCK_IDX(0)

#define WRITE_LOCK_IDX(idx)			WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name())
#define WRITE_LOCK					WRITE_LOCK_IDX(0)
// ※ WRITE_LOCK_IDX 도 위 @@ 내용과 마찬가지로 추후 수정 필요



/*---------------
	  Crash
---------------*/
#define CRASH(cause)						\
{											\
	printf("CRASH : %s\n", cause);			\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if(!(expr))								\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}
