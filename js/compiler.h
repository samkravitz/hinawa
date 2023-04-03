#pragma once

#include <stack>
#include <vector>

#include "ast/stmt.h"
#include "ast/visitor.h"
#include "chunk.h"
#include "function.h"
#include "opcode.h"
#include "token_type.h"
#include "value.h"

namespace js
{
class Compiler : public CompilerVisitor
{
public:
	Compiler(const std::vector<Stmt *> &);

	Function compile();

private:
	struct FunctionCompiler
	{
		FunctionCompiler(FunctionCompiler *enclosing, Function *function) :
		    enclosing(enclosing),
		    function(function)
		{ }

		FunctionCompiler *enclosing;
		Function *function;
		int scope_depth{ 0 };
		std::vector<Local> locals;
		int local_count() { return locals.size(); }
	} *current;

	std::vector<Stmt *> stmts;

	void init_compiler(FunctionCompiler *);
	void end_compiler();

	// compile statements
	void compile(const BlockStmt &);
	void compile(const VarDecl &);
	void compile(const ExpressionStmt &);
	void compile(const IfStmt &);
	void compile(const ForStmt &);
	void compile(const FunctionDecl &);
	void compile(const EmptyStmt &);
	void compile(const ReturnStmt &);

	// compile expressions
	void compile(const UnaryExpr &);
	void compile(const UpdateExpr &);
	void compile(const BinaryExpr &);
	void compile(const CallExpr &);
	void compile(const MemberExpr &);
	void compile(const Literal &);
	void compile(const Variable &);

	size_t make_constant(Value);
	void emit_byte(u8);
	void emit_bytes(u8, u8);
	void emit_constant(Value);

	inline Function &current_function() { return *current->function; }
};
}
