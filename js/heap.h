#pragma once

#include <utility>

namespace js
{
class Heap
{
public:
	Heap() = default;

	template<class T, typename... Params> T *allocate(Params &&...params)
	{
		return new T(std::forward<Params>(params)...);
	}
};
}
