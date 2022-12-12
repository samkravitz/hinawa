#pragma once

#include <stack>
#include <vector>

#include "chunk.h"
#include "function.h"
#include "opcode.h"
#include "scanner.h"
#include "token_type.h"
#include "value.h"

namespace js
{
enum Precedence : int
{
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_SUBSCRIPT,
	PREC_PRIMARY
};


class Compiler
{
public:
	Compiler(const char *);

	Function compile();

	// expressions
	void anonymous(bool);
	void array(bool);
	void binary(bool);
	void call(bool);
	void dot(bool);
	void grouping(bool);
	void literal(bool);
	void number(bool);
	void object(bool);
	void string(bool);
	void subscript(bool);
	void unary(bool);
	void variable(bool);

private:
	Scanner scanner;
	Token current;
	Token previous;
	std::stack<Function> functions;

	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);
	bool check(TokenType);

	void parse_precedence(Precedence);

	// declarations
	void declaration();
	void class_declaration();
	void function_declaration();
	void var_declaration();

	// statements
	void statement();
	void block();
	void expression_statement();
	void if_statement();
	void return_statement();
	void try_statement();
	void while_statement();

	void expression();

	size_t make_constant(Value);
	void emit_byte(u8);
	void emit_bytes(u8, u8);
	void emit_constant(Value);
	size_t emit_jump(Opcode);
	void patch_jump(size_t);
	void emit_loop(size_t);

	void error(const char *);
	void error_at_current(const char *);
	void error_at(Token, const char *);

	u8 parse_variable(const char *);
	void define_variable(u8);
	void declare_variable();
	u8 identifier_constant(Token const &);
	void mark_initialized();

	void add_local(Token);
	int resolve_local(Token);
	void begin_scope();
	void end_scope();

	bool is_global();

	inline Function &current_function() { return functions.top(); }
};
}
