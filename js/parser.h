#pragma once

#include <memory>
#include <string>

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

	std::shared_ptr<AstNode> parse();

private:
	Scanner scanner;
	Token current_token;
	Token previous_token;

	// parse statements
	std::shared_ptr<Stmt> statement();
	std::shared_ptr<Stmt> expression_statement();

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
	bool match(std::initializer_list<TokenType> const &);
	TokenType peek();
};
};
