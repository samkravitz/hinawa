#pragma once

#include <functional>
#include <string>
#include <vector>

#include "ast/expr.h"
#include "ast/stmt.h"
#include "scanner.h"
#include "token.h"

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

class Parser;
struct ParseRule
{
	std::function<Expr *(Parser *)> prefix;
	std::function<Expr *(Parser *, Expr *)> infix;
	Precedence precedence;
};

class Parser
{
public:
	Parser(std::string);

	std::vector<Stmt *> parse();

	Scanner scanner;
	Token current;
	Token previous;

	// parse expressions
	Expr *expression();
	Expr *anonymous();
	Expr *array();
	Expr *assign(Expr *);
	Expr *binary(Expr *);
	Expr *call(Expr *);
	Expr *dot(Expr *);
	Expr *grouping();
	Expr *literal();
	Expr *new_instance();
	Expr *number();
	Expr *object();
	Expr *string();
	Expr *subscript(Expr *);
	Expr *unary();
	Expr *update(Expr *);
	Expr *variable();

private:
	// parse statements
	Stmt *statement();
	Stmt *block_stmt();
	Stmt *expression_statement();
	Stmt *if_statement();
	Stmt *return_statement();
	Stmt *for_statement();
	Stmt *throw_statement();

	// parse declarations
	Stmt *declaration();
	Stmt *function_declaration();
	Stmt *var_declaration();

	Expr *parse_precedence(Precedence);
	ParseRule get_rule(TokenType);
	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);
	bool match_any(std::initializer_list<TokenType> const &);
	TokenType peek();
	inline bool check(TokenType type) { return current.type() == type; }
	bool check_any(std::initializer_list<TokenType> const &);
};
};
