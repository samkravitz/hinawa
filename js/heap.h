#pragma once

#include <string>
#include <utility>
#include <vector>

#include "cell.h"
#include "object.h"
#include "primitive_string.h"
#include <fmt/format.h>

#define DEBUG_STRESS_GC

namespace js
{
class Vm;

class Heap
{
public:
	Heap() = default;

	template<class T, typename... Params> T *allocate(Params &&...params)
	{
		static_assert(std::is_base_of<Cell, T>::value, "T not derived from Object");

#ifdef DEBUG_STRESS_GC
		collect_garbage();
#endif
		auto *cell = new T(std::forward<Params>(params)...);

#ifdef DEBUG_LOG_GC
		fmt::print("{} allocate {} for {}\n", (void *) cell, sizeof(T), cell->to_string());
#endif

		cell->next = cells;
		cells = cell;
		return cell;
	}

	PrimitiveString *allocate_string(std::string str) { return allocate<PrimitiveString>(str); }

	// allocates an empty object, {}
	Object *allocate() { return allocate<Object>(); }

	void set_vm(Vm &vm) { m_vm = &vm; }
	bool has_vm() const { return m_vm != nullptr; }
	Vm &vm() { return *m_vm; }

private:
	Cell *cells = nullptr;
	Vm *m_vm = nullptr;
	std::vector<Cell *> gray_cells;

	void collect_garbage();
	void mark();
	void trace();

	void blacken_cell(Cell *);
	void mark_value(Value);
	void mark_cell(Cell *);
};

extern Heap g_heap;
Heap &heap();
}
