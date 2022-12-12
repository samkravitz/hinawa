#include "function.h"

namespace js
{
Function::Function(std::string const &name) :
    name(name)
{
	arity = 0;
}
}
