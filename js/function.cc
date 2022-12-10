#include "chunk.h"

namespace js
{
Function::Function(std::string const &name) :
    name(name)
{
	num_params = 0;
	native = false;
}
}
