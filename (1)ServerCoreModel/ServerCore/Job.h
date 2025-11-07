/* -------------------------------------
	  (1)ServerCoreModel_250528
--------------------------------------*/
#pragma once

#include <functional>
/*-------------
	  Job
-------------*/

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback)) {}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		// 기존 강의 코드
		{
			/*
			_callback = [owner, memFunc, args...]()
				{
					(owner.get()->*memFunc)(args...);
				};

				*/
		}
		/* ※ refCount ++
			-> 기존 강의 코드는 위와 같았는데, 잘못된 코드. CallbackType이 std::function<void()>; 라 해도, shared_ptr인 owner
			   를 담으면, _callback이 refCount를 점유
			   따라서 아래처럼 _callback에 weak_ptr<T>.lock()으로 해제하여 시행하는 코드로 수정함
		*/



		// 수정 코드
		{
			weak_ptr<T> weakOwner = owner;

			auto tupleArgs = std::make_tuple(std::forward<Args>(args)...);

			_callback = [weakOwner, memFunc, tupleArgs = std::move(tupleArgs)]() mutable
				{
					if (shared_ptr<T> owner = weakOwner.lock())
					{
						std::apply(
							[&](auto&&... unpackedArgs)
							{
								((*owner).*memFunc)(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
							},
							tupleArgs
						);
					}
				};

			/* ○ std::apply 와 튜플
			   - std::apply는 인자를 std::forward<Args>(args)...로 받지 못하고, 튜플로 받음.
			   - 따라서 위처럼 튜플로 변환한 뒤에, tuple인자를 할당
			   - apply는 인자 2개를 받는데, (1) : 함수(람다식), (2) : 튜플
			   - 함수(람다식)의 auto&&... unpackedArgs는, 튜플인 tupleArgs의 요소들을, apply가 하나 하나 변환하여, unpackedArgs에 할당

			   ○ mutable
			    - 람다식의 [a,b]() ... 에서 a,b로 할시, a,b 는 const로 수정 불가. apply는 tupleArgs(튜플)을 auto&&... 로 std::move
				  하기 때문에, tupleArgs의 수정이 필요하기 때문에 mutable로 a,b를 수정할 수 있도록 함

			   ○ decltype
			    - 어떤 표현식의 타입을 추론해서 그대로 가져오는 키워드
				 -> ex). int a = 10; decltype(a) b = 20; -> b의 타입은 int
				- 위처럼, Ret(T::* memFunc)(Args...) 로 받은 memFunc의 리턴값을 추론하여 반환하는 것에 사용
			*/
		}

	}

	void Execute() { _callback(); }

private:
	CallbackType _callback;
};