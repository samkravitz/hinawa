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
public:
	explicit ComponentValue(Token t) : data(t) {}
	explicit ComponentValue(Function f) : data(f) {}
	explicit ComponentValue(ParserBlock b) : data(b) {}

private:
	std::variant<Token, Function, ParserBlock> data;
};
}
