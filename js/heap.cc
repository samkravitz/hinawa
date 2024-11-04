#include "heap.h"

#include <cassert>

#include "vm.h"

namespace js
{
Heap g_heap = {};

Heap &heap()
{
	return g_heap;
}

String *Heap::allocate_string(std::string str)
{
	auto hash = String::hash_string(str);
	if (strings.contains(hash))
		return strings[hash];

	auto *string = allocate<String>(str);
	strings[hash] = string;
	return string;
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
	sweep();

#ifdef DEBUG_LOG_GC
	fmt::print("-- gc end\n");
#endif
}

// mark roots
void Heap::mark()
{
	// mark the global object as clear
	mark_cell(vm().m_global);

	// mark values on the vm's value stack
	for (auto &value : vm().stack)
		mark_value(value);

	// mark closures and upvalues on vm's call stack
	for (auto &frame : vm().call_stack)
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
		gray_cells.pop_back();
		blacken_cell(cell);
	}
}

void Heap::sweep()
{
	Cell *prev = nullptr;
	Cell *cell = cells;

	while (cell)
	{
		if (cell->marked)
		{
			cell->marked = false;
			prev = cell;
			cell = cell->next;
		}

		else
		{
			Cell *swept = cell;
			cell = cell->next;

			if (prev)
				prev->next = cell;
			else
				cells = cell;

			fmt::print("sweep {}\n", swept->to_string());

			free_cell(swept);
		}
	}
}

void Heap::blacken_cell(Cell *cell)
{
#ifdef DEBUG_LOG_GC
	fmt::print("{} blacken {}\n", (void *) cell, cell->to_string());
#endif

	if (cell->is_object())
	{
		auto *object = static_cast<Object *>(cell);

		// mark object's properties
		for (const auto &[key, value] : object->own_properties)
			mark_value(value);

		if (object->is_function())
		{
			auto *function = static_cast<Function *>(object);

			// mark function name
			mark_cell(function->name);

			// mark all constants in function
			for (auto value : function->chunk.constants)
				mark_value(value);
		}

		if (object->is_closure())
		{
			auto *closure = static_cast<Closure *>(cell);
			mark_cell(closure->function);
			for (auto *upvalue : closure->upvalues)
				mark_cell(upvalue);
		}
	}
}

void Heap::mark_value(Value value)
{
	if (value.is_string())
		mark_cell(&value.as_string());

	if (value.is_object())
		mark_cell(value.as_object());
}

void Heap::mark_cell(Cell *cell)
{
	if (!cell || cell->marked)
		return;

#ifdef DEBUG_LOG_GC
	fmt::print("{} mark {}\n", (void *) cell, cell->to_string());
#endif

	cell->marked = true;
	gray_cells.push_back(cell);
}

void Heap::free_cell(Cell *cell)
{
	assert(cell);
#ifdef DEBUG_LOG_GC
	fmt::print("{} free {} bytes {}\n", (void *) cell, sizeof(*cell), cell->to_string());
#endif

	bytes_allocated -= sizeof(*cell);
	delete cell;
}
}
