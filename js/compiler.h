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
	Compiler(const std::vector<std::shared_ptr<Stmt>> &);

	Function compile();
	int current_line{0};

private:
	struct FunctionCompiler
	{
		FunctionCompiler(FunctionCompiler *enclosing, Function *function) :
		    enclosing(enclosing),
		    function(function)
		{ }

		struct CompilerUpvalue
		{
			u8 index{0};
			bool is_local{false};
		};

		FunctionCompiler *enclosing{nullptr};
		Function *function;
		int scope_depth{0};
		std::vector<Local> locals;
		std::vector<CompilerUpvalue> upvalues;
		int local_count() { return locals.size(); }
		int upvalue_count() { return upvalues.size(); }
	} *current{nullptr};

	std::vector<std::shared_ptr<Stmt>> stmts;

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
	void compile(const ThrowStmt &);
	void compile(const TryStmt &);

	// compile expressions
	void compile(const UnaryExpr &);
	void compile(const UpdateExpr &);
	void compile(const BinaryExpr &);
	void compile(const AssignmentExpr &);
	void compile(const CallExpr &);
	void compile(const MemberExpr &);
	void compile(const Literal &);
	void compile(const Variable &);
	void compile(const ObjectExpr &);
	void compile(const FunctionExpr &);
	void compile(const NewExpr &);
	void compile(const ArrayExpr &);

	void assignment_target(const Expr &);

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
	int resolve_local(FunctionCompiler *, const std::string &);
	int resolve_upvalue(FunctionCompiler *, const std::string &);
	int add_upvalue(FunctionCompiler *, u8, bool);

	bool is_global() const;

	inline Function &current_function() { return *current->function; }
};
}
