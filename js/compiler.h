#pragma once

#include <vector>

#include "ast/stmt.h"
#include "ast/visitor.h"
#include "function.h"
#include "opcode.h"
#include "value.h"

namespace js
{
class Compiler : public CompilerVisitor
{
public:
	static Function *compile(const std::vector<std::shared_ptr<Stmt>> &);
	int current_line{0};

private:
	Compiler(const std::vector<std::shared_ptr<Stmt>> &);

	Function *compile_impl();

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

	/**
	* Every time a continue statement is encountered while compiling a loop,
	* that statement's bytecode offset will be appended to the loop's targets.
	* loops can be nested, and each nesting will have a different continue target,
	* so that's why this is stored as a vector of vectors.
	*
	* The continue_targets contains a std::pair of <target_address, num_local_vars_at_location_of_continue>,
	* which is necessary because the compiler will need to emit a variable number of OP_POPs when compiling
	* a continue statement.
	*
	* After the loop's body has been compiled, the topmost continue targets will
	* be iterated through and patched so the VM knows the correct address to jump to.
	*/
	std::vector<std::vector<std::pair<size_t, size_t>>> continue_targets;
	std::vector<std::vector<size_t>> break_targets;

	void init_compiler(FunctionCompiler *);
	void end_compiler();

	// compile statements
	void compile(const BlockStmt &);
	void compile(const ScopeNode &);
	void compile(const VarDecl &);
	void compile(const ExpressionStmt &);
	void compile(const IfStmt &);
	void compile(const ForStmt &);
	void compile(const WhileStmt &);
	void compile(const ContinueStmt &);
	void compile(const BreakStmt &);
	void compile(const DebuggerStmt &);
	void compile(const FunctionDecl &);
	void compile(const EmptyStmt &);
	void compile(const ReturnStmt &);
	void compile(const ThrowStmt &);
	void compile(const TryStmt &);
	void compile(const PrintStmt &);

	// compile expressions
	void compile(const UnaryExpr &);
	void compile(const UpdateExpr &);
	void compile(const BinaryExpr &);
	void compile(const LogicalExpr &);
	void compile(const AssignmentExpr &);
	void compile(const CallExpr &);
	void compile(const MemberExpr &);
	void compile(const Literal &);
	void compile(const Variable &);
	void compile(const ObjectExpr &);
	void compile(const FunctionExpr &);
	void compile(const NewExpr &);
	void compile(const ArrayExpr &);
	void compile(const TernaryExpr &);

	void assignment_target(const Expr &);

	size_t make_constant(Value);
	void emit_byte(u8);
	void emit_bytes(u8, u8);
	void emit_constant(Value);
	size_t emit_jump(Opcode);
	void patch_jump(size_t);
	void patch_pop_n(size_t, size_t);
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
