#pragma once

#include <string>
#include <vector>

#include "ast/expr.h"
#include "ast/stmt.h"
#include "scanner.h"
#include "token.h"

namespace js
{
class Parser
{
public:
	Parser(std::string);

	std::vector<Stmt *> parse();

private:
	Scanner scanner;
	Token current_token;
	Token previous_token;

	// parse statements
	Stmt *statement();
	Stmt *block_stmt();
	Stmt *variable_statement();
	Stmt *expression_statement();
	Stmt *if_statement();
	Stmt *return_statement();

	// parse declarations
	Stmt *declaration();
	Stmt *function_declaration();

	// parse expressions
	Expr *expression();
	Expr *equality();
	Expr *comparison();
	Expr *term();
	Expr *factor();
	Expr *unary();
	Expr *call();
	Expr *primary();

	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);
	bool match_any(std::initializer_list<TokenType> const &);
	TokenType peek();
};
};
