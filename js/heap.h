#pragma once

#include <string>
#include <utility>

#include "cell.h"
#include "object.h"
#include "primitive_string.h"

namespace js
{
class Heap
{
public:
	Heap() = default;

	template<class T, typename... Params> T *allocate(Params &&...params)
	{
		static_assert(std::is_base_of<Cell, T>::value, "T not derived from Object");

		auto *cell = new T(std::forward<Params>(params)...);
		cell->next = cells;
		cells = cell;
		return cell;
	}

	PrimitiveString *allocate_string(std::string str)
	{
		return allocate<PrimitiveString>(str);
	}

	// allocates an empty object, {}
	Object *allocate() { return allocate<Object>(); }

private:
	Cell *cells = nullptr;
};

extern Heap g_heap;
Heap &heap();
}
