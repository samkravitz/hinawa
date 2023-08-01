#pragma once

#include <string>
#include <utility>

#include "object.h"
#include "object_string.h"

namespace js
{
class Heap
{
public:
	Heap() = default;

	template<class T, typename... Params> T *allocate(Params &&...params)
	{
		static_assert(std::is_base_of<Object, T>::value, "T not derived from Object");

		auto *object = new T(std::forward<Params>(params)...);
		object->next = objects;
		objects = object;
		return object;
	}

	//template<> Object *allocate<std::string>(const std::string &str)
	//{
	//	auto *object = new std(std::forward<Params>(params)...);
	//	object->next = objects;
	//	objects = object;
	//	return object;
	//}

	// allocates an empty object, {}
	Object *allocate() { return allocate<Object>(); }

private:
	Object *objects = nullptr;
};

extern Heap g_heap;
Heap &heap();
}
