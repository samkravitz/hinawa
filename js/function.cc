#include "function.h"

#include "object.h"
#include "vm.h"

namespace js
{
NativeFunction *NativeFunction::create(const std::function<Value(Vm &vm, const std::vector<Value>)> &fn)
{
	auto *native = heap().allocate<NativeFunction>(fn);

	if (heap().has_vm())
		heap().vm().push(Value(native));

	/**
	* functions have a property "prototype", that is an object
	* with the property "constructor", which holds a reference
	* to the function. When a new instance is created from the
	* function with the 'new' keyword, the created object's
	* prototype is the beforementioned object.
	*/
	auto *object = heap().allocate();
	object->set("constructor", Value(native));
	native->set("prototype", Value(object));

	if (heap().has_vm())
		heap().vm().pop();
	return native;
}

Closure *Closure::create(Function *function)
{
	auto *closure = heap().allocate<Closure>(function);

	if (heap().has_vm())
		heap().vm().push(Value(closure));

	/**
	* functions have a property "prototype", that is an object
	* with the property "constructor", which holds a reference
	* to the function. When a new instance is created from the
	* function with the 'new' keyword, the created object's
	* prototype is the beforementioned object.
	*/
	auto *object = heap().allocate();
	object->set("constructor", Value(closure));
	closure->set("prototype", Value(object));

	if (heap().has_vm())
		heap().vm().pop();
	return closure;
}
}
