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
		}
	}

	void Execute() { _callback(); }

private:
	CallbackType _callback;
};