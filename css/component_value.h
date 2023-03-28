#pragma once

#include <variant>

#include "parser.h"
#include "token.h"

namespace css
{
class Function
{ };

class ComponentValue
{
	ComponentValue() :
	    token({})
	{ }

	explicit ComponentValue(const Token &t) :
	    token(t)
	{ }

private:
	std::variant<Token, Function, ParserBlock> data;
};
}
