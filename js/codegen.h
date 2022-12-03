#pragma once

#include "ast/expr.h"
#include "ast/stmt.h"
#include "chunk.h"

namespace js
{
Chunk codegen(std::vector<Stmt *> const &);
}
