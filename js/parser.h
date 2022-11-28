#pragma once

#include <memory>
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

	std::vector<std::shared_ptr<Stmt>> parse();

private:
	Scanner scanner;
	Token current_token;
	Token previous_token;

	// parse statements
	std::shared_ptr<Stmt> statement();
	std::shared_ptr<Stmt> block_stmt();
	std::shared_ptr<Stmt> variable_statement();
	std::shared_ptr<Stmt> expression_statement();
	std::shared_ptr<Stmt> if_statement();
	std::shared_ptr<Stmt> return_statement();

	// parse declarations
	std::shared_ptr<Stmt> declaration();
	std::shared_ptr<Stmt> function_declaration();

	// parse expressions
	std::shared_ptr<Expr> expression();
	std::shared_ptr<Expr> equality();
	std::shared_ptr<Expr> comparison();
	std::shared_ptr<Expr> term();
	std::shared_ptr<Expr> factor();
	std::shared_ptr<Expr> unary();
	std::shared_ptr<Expr> primary();

	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);
	bool match_any(std::initializer_list<TokenType> const &);
	TokenType peek();
};
};
