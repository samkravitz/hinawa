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

std::vector<Stmt *> Parser::parse()
{
	std::vector<Stmt *> program;
	Stmt *stmt;

	while ((stmt = statement()))
		program.push_back(stmt);

	return program;
}

Stmt *Parser::statement()
{
	if (match(LEFT_BRACE))
		return block_stmt();

	if (match(KEY_VAR))
		return variable_statement();

	if (match(KEY_IF))
		return if_statement();

	if (match(KEY_RETURN))
		return return_statement();

	auto decl = declaration();
	if (decl)
		return decl;

	return expression_statement();
}

Stmt *Parser::block_stmt()
{
	std::vector<Stmt *> stmts;

	while (!match(RIGHT_BRACE))
		stmts.push_back(statement());

	return new BlockStmt(stmts);
}

Stmt *Parser::variable_statement()
{
	consume(IDENTIFIER, "Expected variable name");

	auto identifier = previous_token.value();
	Expr *initializer = nullptr;

	if (match(EQUAL))
		initializer = expression();

	// match optional semicolon after expression statement
	match(SEMICOLON);

	return new VariableStmt(identifier, initializer);
}

Stmt *Parser::expression_statement()
{
	Stmt *stmt = nullptr;
	Expr *expr;
	if ((expr = expression()))
	{
		// match optional semicolon after expression statement
		match(SEMICOLON);

		stmt = new ExpressionStmt(expr);
	}

	return stmt;
}

Stmt *Parser::if_statement()
{
	consume(LEFT_PAREN, "Expected '('");
	Expr *condition = expression();
	consume(RIGHT_PAREN, "Expected ')'");

	Stmt *then_stmt = statement();
	Stmt *else_stmt = nullptr;

	if (match(KEY_ELSE))
		else_stmt = statement();

	return new IfStmt(condition, then_stmt, else_stmt);
}

Stmt *Parser::return_statement()
{
	auto expr = expression();

	// match optional semicolon after return statement
	match(SEMICOLON);

	return new ReturnStmt(expr);
}

Stmt *Parser::declaration()
{
	return function_declaration();
}

Stmt *Parser::function_declaration()
{
	if (!match(KEY_FUNCTION))
		return nullptr;

	consume(IDENTIFIER, "Expected identifier");
	auto name = previous_token.value();
	consume(LEFT_PAREN, "Expected '('");
	consume(RIGHT_PAREN, "Expected ')'");
	consume(LEFT_BRACE, "Expected '{'");
	auto block = block_stmt();

	return new FunctionDecl(name, block);
}

Expr *Parser::expression()
{
	return equality();
}

Expr *Parser::equality()
{
	auto expr = comparison();
	while (match_any({ BANG_EQUAL, EQUAL_EQUAL, EQUAL_EQUAL_EQUAL, BANG_EQUAL_EQUAL }))
	{
		auto op = previous_token;
		auto rhs = comparison();
		expr = new BinaryExpr(expr, op, rhs);
	}
	return expr;
}

Expr *Parser::comparison()
{
	auto expr = term();
	while (match_any({ GREATER, LESS, LESS_EQUAL, GREATER_EQUAL }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = new BinaryExpr(expr, op, rhs);
	}
	return expr;
}

Expr *Parser::term()
{
	auto expr = factor();

	while (match_any({ MINUS, PLUS }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = new BinaryExpr(expr, op, rhs);
	}

	return expr;
}

Expr *Parser::factor()
{
	auto expr = unary();

	while (match_any({ SLASH, STAR }))
	{
		auto op = previous_token;
		auto rhs = term();
		expr = new BinaryExpr(expr, op, rhs);
	}

	return expr;
}

Expr *Parser::unary()
{
	if (match_any({ BANG, MINUS, MINUS_MINUS, PLUS_PLUS }))
	{
		auto op = previous_token;
		auto rhs = unary();
		return new UnaryExpr(op, rhs);
	}

	return call();
}

Expr *Parser::call()
{
	auto expr = primary();

	auto call_helper = [this](Expr *callee) -> Expr *
	{
		std::vector<Expr *> args;

		if (peek() != RIGHT_PAREN)
		{
			do
			{
				args.push_back(expression());
			} while (match(COMMA));
		}

		return new CallExpr(callee, args);
	};

	while (true)
	{
		if (match(LEFT_PAREN))
			expr = call_helper(expr);
		else
			break;
	}

	return expr;
}

Expr *Parser::primary()
{
	if (match(NUMBER))
		return new Literal(Value(std::stof(previous_token.value())));

	if (match(IDENTIFIER))
		return new Variable(previous_token.value());

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
