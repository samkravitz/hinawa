#include "parser.h"

#include <cassert>
#include <iostream>
#include <sstream>

#include "token_type.h"

namespace js
{
Parser::Parser(std::string input) :
    scanner(input.c_str())
{
	advance();
}

std::shared_ptr<AstNode> Parser::parse()
{
	auto program = std::make_shared<Program>();
	std::shared_ptr<Stmt> stmt;

	while ((stmt = statement())) {
		program->add_stmt(stmt);
	}

	return program;
}

std::shared_ptr<Stmt> Parser::statement()
{
	return expression_statement();
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
	std::shared_ptr<Stmt> stmt = nullptr;
	std::shared_ptr<Expr> expr;
	if ((expr = expression()))
		stmt = std::make_shared<ExpressionStmt>(expr);

	return stmt;
}

std::shared_ptr<Expr> Parser::expression()
{
	return equality();
}

std::shared_ptr<Expr> Parser::equality()
{
	auto expr = comparison();

	return expr;
}

std::shared_ptr<Expr> Parser::comparison()
{
	auto expr = term();
	return expr;
}

std::shared_ptr<Expr> Parser::term()
{
	auto expr = factor();

	while (match({ MINUS, PLUS }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = std::make_shared<BinaryExpr>(expr, BinaryExpr::BinaryOp::Plus, rhs);
	}

	return expr;
}

std::shared_ptr<Expr> Parser::factor()
{
	auto expr = unary();

	while (match({ SLASH, STAR }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = std::make_shared<BinaryExpr>(expr, BinaryExpr::BinaryOp::Plus, rhs);
	}

	return expr;
}

std::shared_ptr<Expr> Parser::unary()
{
	return primary();
}

std::shared_ptr<Expr> Parser::primary()
{
	if (match(NUMBER))
	{
		return std::make_shared<Literal>(Value(std::stof(previous_token.value())));
	}

	return nullptr;
}

void Parser::advance()
{
	previous_token = current_token;
	current_token = scanner.next();

	std::cout << current_token.to_string() << "\n";
}

bool Parser::match(TokenType type)
{
	if (current_token.type() == type)
	{
		advance();
		return true;
	}

	return false;
}

bool Parser::match(std::initializer_list<TokenType> const &list)
{
	for (auto t : list)
	{
		if (match(t))
			return true;
	}

	return false;
}

void Parser::consume(TokenType type, const char *msg)
{
	if (!match(type))
	{
		std::cout << msg << "\n";
	}
}

TokenType Parser::peek()
{
	return current_token.type();
}
}
