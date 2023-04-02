#include "parser.h"

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <sstream>

#include "ast/expr.h"
#include "token_type.h"

namespace js
{
struct ParseRule
{
	std::function<Expr *(Parser *)> prefix;
	std::function<Expr *(Parser *, Expr *)> infix;
	Precedence precedence;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
ParseRule rules[] = {
	[0]                 = { nullptr, nullptr, PREC_NONE },
	[LEFT_PAREN]        = { &Parser::grouping, &Parser::call, PREC_CALL },
	[RIGHT_PAREN]       = { nullptr, nullptr, PREC_NONE },
	[LEFT_BRACE]        = { &Parser::object, nullptr, PREC_SUBSCRIPT },
	[RIGHT_BRACE]       = { nullptr, nullptr, PREC_NONE },
	[LEFT_BRACKET]      = { &Parser::array, &Parser::subscript, PREC_SUBSCRIPT },
	[RIGHT_BRACKET]     = { nullptr, nullptr, PREC_NONE },
	[COMMA]             = { nullptr, nullptr, PREC_NONE },
	[DOT]               = { nullptr, &Parser::dot, PREC_CALL },
	[MINUS]             = { &Parser::unary, &Parser::binary, PREC_TERM },
	[PLUS]              = { nullptr, &Parser::binary, PREC_TERM },
	[SLASH]             = { nullptr, &Parser::binary, PREC_FACTOR },
	[STAR]              = { nullptr, &Parser::binary, PREC_FACTOR },
	[MOD]               = { nullptr, &Parser::binary, PREC_FACTOR },
	[SEMICOLON]         = { nullptr, nullptr, PREC_NONE },
	[COLON]             = { nullptr, nullptr, PREC_NONE },
	[BANG]              = { nullptr, nullptr, PREC_NONE },
	[EQUAL]             = { nullptr, nullptr, PREC_NONE },
	[GREATER]           = { nullptr, &Parser::binary, PREC_COMPARISON },
	[LESS]              = { nullptr, &Parser::binary, PREC_COMPARISON },
	[AND]               = { nullptr, &Parser::binary, PREC_TERM },
	[PIPE]              = { nullptr, &Parser::binary, PREC_TERM },
	[TILDE]             = { nullptr, &Parser::binary, PREC_TERM },
	[CARET]             = { nullptr, &Parser::binary, PREC_TERM },
	[QUESTION]          = { nullptr, &Parser::binary, PREC_TERM },
	[NEWLINE]           = { nullptr, nullptr, PREC_NONE },

	[BANG_EQUAL]        = { nullptr, &Parser::binary, PREC_EQUALITY },
	[EQUAL_EQUAL]       = { nullptr, &Parser::binary, PREC_COMPARISON },
	[GREATER_EQUAL]     = { nullptr, &Parser::binary, PREC_COMPARISON },
	[LESS_EQUAL]        = { nullptr, &Parser::binary, PREC_COMPARISON },
	[PLUS_EQUAL]        = { nullptr, &Parser::binary, PREC_COMPARISON },
	[MINUS_EQUAL]       = { nullptr, &Parser::binary, PREC_COMPARISON },
	[STAR_EQUAL]        = { nullptr, &Parser::binary, PREC_COMPARISON },
	[SLASH_EQUAL]       = { nullptr, &Parser::binary, PREC_COMPARISON },
	[AND_EQUAL]         = { nullptr, &Parser::binary, PREC_COMPARISON },
	[PIPE_EQUAL]        = { nullptr, &Parser::binary, PREC_COMPARISON },
	[CARET_EQUAL]       = { nullptr, &Parser::binary, PREC_COMPARISON },
	[LESS_LESS]         = { nullptr, nullptr, PREC_NONE },
	[GREATER_GREATER]   = { nullptr, nullptr, PREC_NONE },
	[AND_AND]           = { nullptr, &Parser::binary, PREC_AND },
	[PIPE_PIPE]         = { nullptr, &Parser::binary, PREC_OR },
	[ARROW]             = { nullptr, &Parser::binary, PREC_AND },
	[QUESTION_QUESTION] = { nullptr, &Parser::binary, PREC_AND },
	[STAR_STAR]         = { nullptr, &Parser::binary, PREC_AND },
	[PLUS_PLUS]         = { &Parser::unary, &Parser::inc_dec, PREC_UNARY },
	[MINUS_MINUS]       = { &Parser::unary, &Parser::inc_dec, PREC_UNARY },
	[QUESTION_DOT]      = { nullptr, &Parser::binary, PREC_AND },

	[EQUAL_EQUAL_EQUAL] = { nullptr, &Parser::binary, PREC_EQUALITY },
	[BANG_EQUAL_EQUAL]  = { nullptr, &Parser::binary, PREC_EQUALITY },
	[STAR_STAR_EQUAL]   = { nullptr, &Parser::binary, PREC_EQUALITY },
	[LESS_LESS_EQUAL]   = { nullptr, &Parser::binary, PREC_EQUALITY },
	[RIGHT_RIGHT_EQUAL] = { nullptr, &Parser::binary, PREC_EQUALITY },
	[AND_AND_EQUAL]     = { nullptr, &Parser::binary, PREC_EQUALITY },
	[PIPE_PIPE_EQUAL]   = { nullptr, &Parser::binary, PREC_EQUALITY },
	[RIGHT_RIGHT_RIGHT] = { nullptr, &Parser::binary, PREC_EQUALITY },
	[DOT_DOT_DOT]       = { nullptr, &Parser::binary, PREC_EQUALITY },

	[IDENTIFIER]        = { &Parser::variable, nullptr, PREC_NONE },
	[STRING]            = { &Parser::string, nullptr, PREC_NONE },
	[NUMBER]            = { &Parser::number, nullptr, PREC_NONE },

	[KEY_AWAIT]         = { nullptr, nullptr, PREC_NONE },
	[KEY_BREAK]         = { nullptr, nullptr, PREC_NONE },
	[KEY_CASE]          = { nullptr, nullptr, PREC_NONE },
	[KEY_CATCH]         = { nullptr, nullptr, PREC_NONE },
	[KEY_CLASS]         = { nullptr, nullptr, PREC_NONE },
	[KEY_CONST]         = { nullptr, nullptr, PREC_NONE },
	[KEY_CONTINUE]      = { nullptr, nullptr, PREC_NONE },
	[KEY_DEBUGGER]      = { nullptr, nullptr, PREC_NONE },
	[KEY_DEFAULT]       = { nullptr, nullptr, PREC_NONE },
	[KEY_DELETE]        = { nullptr, nullptr, PREC_NONE },
	[KEY_DO]            = { nullptr, nullptr, PREC_NONE },
	[KEY_ELSE]          = { nullptr, nullptr, PREC_NONE },
	[KEY_ENUM]          = { nullptr, nullptr, PREC_NONE },
	[KEY_EXPORT]        = { nullptr, nullptr, PREC_NONE },
	[KEY_EXTENDS]       = { nullptr, nullptr, PREC_NONE },
	[KEY_FALSE]         = { &Parser::literal, nullptr, PREC_NONE },
	[KEY_FINALLY]       = { nullptr, nullptr, PREC_NONE },
	[KEY_FOR]           = { nullptr, nullptr, PREC_NONE },
	[KEY_FUNCTION]      = { &Parser::anonymous, nullptr, PREC_NONE },
	[KEY_IF]            = { nullptr, nullptr, PREC_NONE },
	[KEY_IMPLEMENTS]    = { nullptr, nullptr, PREC_NONE },
	[KEY_IMPORT]        = { nullptr, nullptr, PREC_NONE },
	[KEY_IN]            = { nullptr, nullptr, PREC_NONE },
	[KEY_INSTANCEOF]    = { nullptr, nullptr, PREC_NONE },
	[KEY_INTERFACE]     = { nullptr, nullptr, PREC_NONE },
	[KEY_LET]           = { nullptr, nullptr, PREC_NONE },
	[KEY_NEW]           = { &Parser::new_instance, nullptr, PREC_NONE },
	[KEY_NULL]          = { &Parser::literal, nullptr, PREC_NONE },
	[KEY_PACKAGE]       = { nullptr, nullptr, PREC_NONE },
	[KEY_PRIVATE]       = { nullptr, nullptr, PREC_NONE },
	[KEY_PROTECTED]     = { nullptr, nullptr, PREC_NONE },
	[KEY_PUBLIC]        = { nullptr, nullptr, PREC_NONE },
	[KEY_RETURN]        = { nullptr, nullptr, PREC_NONE },
	[KEY_SUPER]         = { nullptr, nullptr, PREC_NONE },
	[KEY_SWITCH]        = { nullptr, nullptr, PREC_NONE },
	[KEY_STATIC]        = { nullptr, nullptr, PREC_NONE },
	[KEY_THIS]          = { nullptr, nullptr, PREC_NONE },
	[KEY_THROW]         = { nullptr, nullptr, PREC_NONE },
	[KEY_TRY]           = { nullptr, nullptr, PREC_NONE },
	[KEY_TRUE]          = { &Parser::literal, nullptr, PREC_NONE },
	[KEY_TYPEOF]        = { nullptr, nullptr, PREC_NONE },
	[KEY_UNDEFINED]     = { &Parser::literal, nullptr, PREC_NONE },
	[KEY_VAR]           = { nullptr, nullptr, PREC_NONE },
	[KEY_VOID]          = { nullptr, nullptr, PREC_NONE },
	[KEY_WHILE]         = { nullptr, nullptr, PREC_NONE },
	[KEY_WITH]          = { nullptr, nullptr, PREC_NONE },
	[KEY_YIELD]         = { nullptr, nullptr, PREC_NONE },

	[TOKEN_EOF]         = { nullptr, nullptr, PREC_NONE },
};
#pragma GCC diagnostic pop

static ParseRule *get_rule(TokenType type)
{
	return &rules[type];
}

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

	if (match(KEY_FOR))
		return for_statement();

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

	auto identifier = previous.value();
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

Stmt *Parser::for_statement()
{
	consume(LEFT_PAREN, "Expected '('");

	AstNode *initialization = nullptr;
	if (match(KEY_VAR))
	{
		initialization = variable_statement();
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

Stmt *Parser::declaration()
{
	return function_declaration();
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

Expr *Parser::binary(Expr *left)
{
	auto op = previous;
	auto precedence = get_rule(op.type())->precedence;
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
	return nullptr;
}

Expr *Parser::grouping()
{
	return nullptr;
}

Expr *Parser::inc_dec(Expr *left)
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
	auto d = std::stod(previous.value());
	return new Literal(Value(d));
}

Expr *Parser::object()
{
	return nullptr;
}

Expr *Parser::string()
{
	auto str = previous.value();
	return new Literal(Value(new std::string(str.substr(1, str.size() - 2))));
}

Expr *Parser::subscript(Expr *left)
{
	return nullptr;
}

Expr *Parser::unary()
{
	return nullptr;
}

Expr *Parser::variable()
{
	return new Variable(previous.value());
}

Expr *Parser::parse_precedence(Precedence precedence)
{
	advance();
	auto prefix = get_rule(previous.type())->prefix;
	if (!prefix)
	{
		std::cerr << "Expect expression\n";
		return nullptr;
	}

	Expr *expr = prefix(this);

	while (precedence <= get_rule(current.type())->precedence)
	{
		advance();
		auto infix = get_rule(previous.type())->infix;
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
