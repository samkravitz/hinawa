#include "heap.h"


#include "vm.h"

namespace js
{
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

	mark();
	trace();

#ifdef DEBUG_LOG_GC
	fmt::print("-- gc end\n");
#endif
}

void Heap::mark()
{
	// mark the global object as clear
	mark_cell(vm().global);

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

void Heap::trace()
{
	while (!gray_cells.empty())
	{
		auto *cell = gray_cells.back();
		blacken_cell(cell);
		gray_cells.pop_back();
	}
}

void Heap::blacken_cell(Cell *cell) { }

void Heap::mark_value(Value value)
{
	if (value.is_string())
		mark_cell(&value.as_string());

	if (value.is_object())
		mark_cell(value.as_object());
}

void Heap::mark_cell(Cell *cell)
{
	if (!cell)
		return;

#ifdef DEBUG_LOG_GC
	fmt::print("{} mark {}\n", (void *) cell, cell->to_string());
#endif

	cell->marked = true;
	gray_cells.push_back(cell);
}
}