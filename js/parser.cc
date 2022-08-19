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

	while ((stmt = statement()))
		program->add_stmt(stmt);

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
	{
		// match optional semicolon after expression statement
		match(SEMICOLON);

		stmt = std::make_shared<ExpressionStmt>(expr);
	}

	return stmt;
}

std::shared_ptr<Expr> Parser::expression()
{
	return equality();
}

std::shared_ptr<Expr> Parser::equality()
{
	auto expr = comparison();
	while (match_any({ BANG_EQUAL, EQUAL_EQUAL, EQUAL_EQUAL_EQUAL, BANG_EQUAL_EQUAL }))
	{
		auto op = previous_token;
		auto rhs = comparison();
		expr = std::make_shared<BinaryExpr>(expr, op, rhs);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::comparison()
{
	auto expr = term();
	while (match_any({ GREATER, LESS, LESS_EQUAL, GREATER_EQUAL }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = std::make_shared<BinaryExpr>(expr, op, rhs);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::term()
{
	auto expr = factor();

	while (match_any({ MINUS, PLUS }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = std::make_shared<BinaryExpr>(expr, op, rhs);
	}

	return expr;
}

std::shared_ptr<Expr> Parser::factor()
{
	auto expr = unary();

	while (match_any({ SLASH, STAR }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = std::make_shared<BinaryExpr>(expr, op, rhs);
	}

	return expr;
}

std::shared_ptr<Expr> Parser::unary()
{
	if (match_any({ BANG, MINUS, MINUS_MINUS, PLUS_PLUS }))
	{
		auto op = previous_token;
		auto rhs = unary();
		return std::make_shared<UnaryExpr>(op, rhs);
	}

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

bool Parser::match_any(std::initializer_list<TokenType> const &tokens)
{
	for (auto t : tokens)
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
