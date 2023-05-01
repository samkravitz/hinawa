#pragma once

#include <variant>

#include "token.h"

namespace css
{
struct ComponentValue
{
	explicit ComponentValue(const Token &t) :
	    token(t)
	{ }

	Token token;
};
}
