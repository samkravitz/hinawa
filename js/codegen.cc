#include "codegen.h"

namespace js
{
Chunk codegen(std::vector<Stmt *> const &ast)
{
	auto chunk = Chunk{};
	for (auto const &node : ast)
		node->generate_bytecode(chunk);

	chunk.write(OP_RETURN);
	return chunk;
}
}
