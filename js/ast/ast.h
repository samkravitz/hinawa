#pragma once

#include <memory>
#include <vector>

#include "chunk.h"
#include "opcode.h"
#include "visitor.h"

namespace js
{
class AstNode
{
public:
	virtual const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
	virtual void generate_bytecode(Chunk &chunk) const { chunk.write(OP_UNKNOWN); }
};
}
