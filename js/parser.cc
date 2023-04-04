#include "parser.h"

#include <cassert>
#include <cstddef>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace js
{
Parser::Parser(std::string input) :
    scanner(input.c_str())
{
	advance();
}

ParseRule Parser::get_rule(TokenType type)
{
	static std::unordered_map<TokenType, ParseRule> rules = {
	    {LEFT_PAREN,        {&Parser::grouping, &Parser::call, PREC_CALL}       },
	    {LEFT_BRACKET,      {&Parser::array, &Parser::subscript, PREC_SUBSCRIPT}},
	    {LEFT_BRACE,        {&Parser::object, nullptr, PREC_SUBSCRIPT}          },
	    {DOT,	           {nullptr, &Parser::dot, PREC_CALL}                  },
	    {MINUS,             {&Parser::unary, &Parser::binary, PREC_TERM}        },
	    {PLUS,              {nullptr, &Parser::binary, PREC_TERM}               },
	    {SLASH,             {nullptr, &Parser::binary, PREC_FACTOR}             },
	    {STAR,              {nullptr, &Parser::binary, PREC_FACTOR}             },
	    {MOD,	           {nullptr, &Parser::binary, PREC_FACTOR}             },
	    {EQUAL,             {nullptr, &Parser::assign, PREC_ASSIGNMENT}         },
	    {GREATER,           {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {LESS,              {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {AND,	           {nullptr, &Parser::binary, PREC_TERM}               },
	    {PIPE,              {nullptr, &Parser::binary, PREC_TERM}               },
	    {TILDE,             {nullptr, &Parser::binary, PREC_TERM}               },
	    {CARET,             {nullptr, &Parser::binary, PREC_TERM}               },
	    {QUESTION,          {nullptr, &Parser::binary, PREC_TERM}               },
	    {BANG_EQUAL,        {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {EQUAL_EQUAL,       {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {GREATER_EQUAL,     {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {LESS_EQUAL,        {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {PLUS_EQUAL,        {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {MINUS_EQUAL,       {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {STAR_EQUAL,        {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {SLASH_EQUAL,       {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {AND_EQUAL,         {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {PIPE_EQUAL,        {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {CARET_EQUAL,       {nullptr, &Parser::binary, PREC_COMPARISON}         },
	    {AND_AND,           {nullptr, &Parser::binary, PREC_AND}                },
	    {PIPE_PIPE,         {nullptr, &Parser::binary, PREC_OR}                 },
	    {ARROW,             {nullptr, &Parser::binary, PREC_AND}                },
	    {QUESTION_QUESTION, {nullptr, &Parser::binary, PREC_AND}                },
	    {STAR_STAR,         {nullptr, &Parser::binary, PREC_AND}                },
	    {PLUS_PLUS,         {&Parser::unary, &Parser::update, PREC_UNARY}       },
	    {MINUS_MINUS,       {&Parser::unary, &Parser::update, PREC_UNARY}       },
	    {QUESTION_DOT,      {nullptr, &Parser::binary, PREC_AND}                },
	    {EQUAL_EQUAL_EQUAL, {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {BANG_EQUAL_EQUAL,  {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {STAR_STAR_EQUAL,   {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {LESS_LESS_EQUAL,   {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {RIGHT_RIGHT_EQUAL, {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {AND_AND_EQUAL,     {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {PIPE_PIPE_EQUAL,   {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {RIGHT_RIGHT_RIGHT, {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {DOT_DOT_DOT,       {nullptr, &Parser::binary, PREC_EQUALITY}           },
	    {IDENTIFIER,        {&Parser::variable, nullptr, PREC_NONE}             },
	    {STRING,            {&Parser::string, nullptr, PREC_NONE}               },
	    {NUMBER,            {&Parser::number, nullptr, PREC_NONE}               },
	    {KEY_FALSE,         {&Parser::literal, nullptr, PREC_NONE}              },
	    {KEY_FUNCTION,      {&Parser::anonymous, nullptr, PREC_NONE}            },
	    {KEY_NEW,           {&Parser::new_instance, nullptr, PREC_NONE}         },
	    {KEY_NULL,          {&Parser::literal, nullptr, PREC_NONE}              },
	    {KEY_TRUE,          {&Parser::literal, nullptr, PREC_NONE}              },
	    {KEY_UNDEFINED,     {&Parser::literal, nullptr, PREC_NONE}              },
	};

	if (!rules.contains(type))
		return {};

	return rules[type];
}

std::vector<Stmt *> Parser::parse()
{
	std::vector<Stmt *> program;

	while (!match(TOKEN_EOF))
	{
		auto *stmt = declaration();
		if (!stmt)
			break;
		program.push_back(stmt);
	}

	return program;
}

Stmt *Parser::declaration()
{
	if (match(KEY_VAR))
		return var_declaration();

	//if (match(KEY_CLASS))
	//	return class_declaration();

	if (match(KEY_FUNCTION))
		return function_declaration();

	return statement();
}

Stmt *Parser::statement()
{
	if (match(LEFT_BRACE))
		return block_stmt();

	if (match(KEY_IF))
		return if_statement();

	if (match(KEY_RETURN))
		return return_statement();

	if (match(KEY_FOR))
		return for_statement();

	return expression_statement();
}

Stmt *Parser::block_stmt()
{
	std::vector<Stmt *> stmts;

	while (!match(RIGHT_BRACE))
		stmts.push_back(declaration());

	return new BlockStmt(stmts);
}

Stmt *Parser::var_declaration()
{
	consume(IDENTIFIER, "Expected variable name");

	auto identifier = previous.value();
	Expr *initializer = nullptr;

	if (match(EQUAL))
		initializer = expression();

	// match optional semicolon after expression statement
	match(SEMICOLON);

	return new VarDecl(identifier, initializer);
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

Stmt *Parser::for_statement()
{
	consume(LEFT_PAREN, "Expected '('");

	AstNode *initialization = nullptr;
	if (match(KEY_VAR))
	{
		initialization = var_declaration();
		if (!initialization)
		{
			std::cout << "For statement: expected variable statement";
			consume(SEMICOLON, "For statement: expect ; after initializer");
		}
	}
	else
	{
		initialization = expression();
		consume(SEMICOLON, "For statement: expect ; after initializer");
	}

	auto *condition = expression();
	consume(SEMICOLON, "For statement: expect ; after condition");
	auto *afterthought = expression();
	consume(RIGHT_PAREN, "Expected ')'");

	auto *stmt = statement();

	return new ForStmt(initialization, condition, afterthought, stmt);
}

Stmt *Parser::function_declaration()
{
	if (!match(KEY_FUNCTION))
		return nullptr;

	consume(IDENTIFIER, "Expected identifier");
	auto name = previous.value();
	consume(LEFT_PAREN, "Expected '('");
	consume(RIGHT_PAREN, "Expected ')'");
	consume(LEFT_BRACE, "Expected '{'");
	auto block = block_stmt();

	return new FunctionDecl(name, block);
}

Expr *Parser::expression()
{
	return parse_precedence(PREC_ASSIGNMENT);
}

Expr *Parser::anonymous()
{
	return nullptr;
}

Expr *Parser::array()
{
	return nullptr;
}

Expr *Parser::assign(Expr *left)
{
	auto right = expression();
	return new AssignmentExpr(left, right);
}

Expr *Parser::binary(Expr *left)
{
	auto op = previous;
	auto precedence = get_rule(op.type()).precedence;
	auto right = parse_precedence(static_cast<Precedence>(precedence + 1));
	return new BinaryExpr(left, op, right);
}

Expr *Parser::call(Expr *left)
{
	std::vector<Expr *> args;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			args.push_back(expression());
			if (args.size() > 0xff)
				std::cerr << "Can't have more than 255 arguments\n";
		} while (match(COMMA));
	}

	consume(RIGHT_PAREN, "Expect ')' after arguments");
	return new CallExpr(left, args);
}

Expr *Parser::dot(Expr *left)
{
	consume(IDENTIFIER, "Expect identifier after '.'");
	auto property_name = previous;
	return new MemberExpr(left, property_name);
}

Expr *Parser::grouping()
{
	return nullptr;
}

Expr *Parser::literal()
{
	return nullptr;
}

Expr *Parser::new_instance()
{
	return nullptr;
}

Expr *Parser::number()
{
	return new Literal(previous);
}

Expr *Parser::object()
{
	return nullptr;
}

Expr *Parser::string()
{
	return new Literal(previous);
}

Expr *Parser::subscript(Expr *left)
{
	return nullptr;
}

Expr *Parser::unary()
{
	auto op = previous;
	auto operand = parse_precedence(PREC_UNARY);
	if (op.type() == PLUS_PLUS || op.type() == MINUS_MINUS)
		return new UpdateExpr(op, operand, true);

	return new UnaryExpr(op, operand);
}

Expr *Parser::update(Expr *left)
{
	auto op = previous;
	return new UpdateExpr(op, left, false);
}

Expr *Parser::variable()
{
	return new Variable(previous.value(), check_any({EQUAL, PLUS_PLUS, MINUS_MINUS}));
}

Expr *Parser::parse_precedence(Precedence precedence)
{
	advance();
	auto prefix = get_rule(previous.type()).prefix;
	if (!prefix)
	{
		std::cerr << "Expect expression\n";
		return nullptr;
	}

	Expr *expr = prefix(this);

	while (precedence <= get_rule(current.type()).precedence)
	{
		advance();
		auto infix = get_rule(previous.type()).infix;
		expr = infix(this, expr);
	}

	return expr;
}

void Parser::advance()
{
	previous = current;
	current = scanner.next();
}

bool Parser::match(TokenType type)
{
	if (current.type() == type)
	{
		advance();
		return true;
	}

	return false;
}

bool Parser::check_any(std::initializer_list<TokenType> const &tokens)
{
	for (auto t : tokens)
	{
		if (check(t))
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
	return current.type();
}
}
