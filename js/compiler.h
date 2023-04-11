#pragma once

#include <queue>
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
		int scope_depth{0};
		std::vector<Local> locals;
		int local_count() { return locals.size(); }
	} *current;

	std::vector<Stmt *> stmts;

	void init_compiler(FunctionCompiler *);
	void end_compiler();

	// compile statements
	std::optional<size_t> compile(const BlockStmt &);
	std::optional<size_t> compile(const VarDecl &);
	std::optional<size_t> compile(const ExpressionStmt &);
	std::optional<size_t> compile(const IfStmt &);
	std::optional<size_t> compile(const ForStmt &);
	std::optional<size_t> compile(const FunctionDecl &);
	std::optional<size_t> compile(const EmptyStmt &);
	std::optional<size_t> compile(const ReturnStmt &);

	// compile expressions
	std::optional<size_t> compile(const UnaryExpr &);
	std::optional<size_t> compile(const UpdateExpr &);
	std::optional<size_t> compile(const BinaryExpr &);
	std::optional<size_t> compile(const AssignmentExpr &);
	std::optional<size_t> compile(const CallExpr &);
	std::optional<size_t> compile(const MemberExpr &);
	std::optional<size_t> compile(const Literal &);
	std::optional<size_t> compile(const Variable &);

	size_t make_constant(Value);
	void emit_byte(u8);
	void emit_bytes(u8, u8);
	void emit_constant(Value);
	size_t emit_jump(Opcode);
	void patch_jump(size_t);
	void emit_loop(size_t);

	u8 parse_variable(const std::string &);
	void define_variable(u8);
	u8 identifier_constant(const std::string &);
	void mark_initialized();

	void begin_scope();
	void end_scope();

	void declare_local(const std::string &);
	int resolve_local(const std::string &);

	bool is_global() const;

	inline Function &current_function() { return *current->function; }
};
}
