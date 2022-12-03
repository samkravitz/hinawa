#include "codegen.h"

namespace js
{
Chunk codegen(std::vector<std::shared_ptr<Stmt>> const &ast)
{
	auto chunk = Chunk{};
	for (auto const &node : ast)
		node->generate_bytecode(chunk);

	return chunk;
}
}
