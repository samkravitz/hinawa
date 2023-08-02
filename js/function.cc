#include "function.h"

namespace js
{
Function::Function(String *name) :
    name(name)
{
	arity = 0;
}
}
