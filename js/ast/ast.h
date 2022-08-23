#pragma once

#include "visitor.h"

#include <iostream>
#include <memory>
#include <vector>

namespace js
{
class AstNode
{
public:
	virtual const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor) const = 0;

	virtual void print(std::string const &prefix, bool is_left);
	virtual void print(std::string const &title = "");
};
}
