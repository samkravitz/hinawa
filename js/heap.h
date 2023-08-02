#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cell.h"
#include "object.h"
#include "string.hh"
#include <fmt/format.h>

// #define DEBUG_STRESS_GC

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
		fmt::print("{} allocate {} bytes for {}\n", (void *) cell, sizeof(T), cell->to_string());
#endif

		bytes_allocated += sizeof(T);
		cell->next = cells;
		cells = cell;
		return cell;
	}

	String *allocate_string(std::string str);

	// allocates an empty object, {}
	Object *allocate() { return allocate<Object>(); }

	void set_vm(Vm &vm) { m_vm = &vm; }
	bool has_vm() const { return m_vm != nullptr; }
	Vm &vm() { return *m_vm; }

private:
	Cell *cells = nullptr;
	Vm *m_vm = nullptr;
	std::vector<Cell *> gray_cells;
	std::size_t bytes_allocated = 0;
	std::unordered_map<u32, String *> strings;

	void collect_garbage();
	void mark();
	void trace();
	void sweep();

	void blacken_cell(Cell *);
	void mark_value(Value);
	void mark_cell(Cell *);
	void free_cell(Cell *);
};

extern Heap g_heap;
Heap &heap();
}
