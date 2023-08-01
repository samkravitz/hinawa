#include "heap.h"

namespace js
{
Heap g_heap = {};

Heap &heap()
{
	return g_heap;
}
}