#pragma once

#include <fmt/format.h>

#include "visitor.h"

namespace js
{
void print_indent(int);

class AstNode
{
public:
	virtual ~AstNode() = default;
	virtual const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
	virtual void accept(CompilerVisitor *compiler) const = 0;

	void print_header(int indent) const
	{
		print_indent(indent);
		fmt::print("{}\n", name());
	}

	int line{-1};
	int col{-1};
};
}
