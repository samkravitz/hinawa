#pragma once

#include <vector>

#include "chunk.h"
#include "opcode.h"
#include "visitor.h"

namespace js
{
class AstNode
{
public:
	virtual ~AstNode() { }
	virtual const char *name() const = 0;
	virtual void accept(const PrintVisitor *visitor, int indent) const = 0;
	virtual u32 generate_bytecode(Chunk &chunk) const
	{
		//chunk.write(OP_UNKNOWN);
		//return Chunk::REG_INVALID;
		return 0;
	}
};
}
