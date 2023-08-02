#include "heap.h"

#include <fmt/format.h>
namespace js
{
	class Upvalue;
Heap g_heap = {};

Heap &heap()
{
	return g_heap;
}

void Heap::collect_garbage()
{
	if (!has_vm())
		return;

#ifdef DEBUG_LOG_GC
	fmt::print("-- gc begin\n");
#endif

	// mark the global object as clear
	mark_cell(vm().global);
	mark_roots();
	trace_references();

#ifdef DEBUG_LOG_GC
	fmt::print("-- gc end\n");
#endif
}

void Heap::mark_roots()
{
	for (auto &value : vm().stack)
		mark_value(value);

	for (auto &frame : vm().frames)
	{
		auto *closure = frame.closure;
		mark_cell(closure);

		for (auto *upvalue : closure->upvalues)
			mark_cell(upvalue);
	}
}

void Heap::trace_references()
{
	while (!grey_cells.empty())
	{
		auto *cell = grey_cells.last();
		grey_cells.pop_back();
		blacken_cell(cell);
	}
}

void Heap::blacken_cell(Cell *cell)
{

}

void mark_value(Value value)
{
	if (value.is_string())
		mark_cell(&value.as_string());

	if (value.is_object())
		mark_cell(value.as_object());
}

void mark_cell(Cell *cell)
{
	if (!cell)
		return;

#ifdef DEBUG_LOG_GC
	fmt::print("{} mark {}\n", (void *) cell, cell->to_string());
#endif

	cell->marked = true;
	gray_stack.push_back(cell);
}
}