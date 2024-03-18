#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ast/expr.h"
#include "ast/stmt.h"
#include "token.h"

namespace js
{
// javascript operator precedence
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence#table
enum Precedence : int
{
	PREC_GROUPING = 18,             // ( ... )
	PREC_CALL = 17,                 // ., .?, [ ... ], new ... (...)
	PREC_NEW = 16,                  // new ...
	PREC_POSTFIX_INCREMENT = 15,    // ... ++, ... --
	PREC_UNARY = 14,                // !, ~, +, -, ++ ..., -- ..., typeof, void, delete, await
	PREC_EXPONENTIATION = 13,       // **
	PREC_FACTOR = 12,               // .. / .., .. * .., .. % ..
	PREC_TERM = 11,                 // .. + .., .. - ..
	PREC_SHIFT = 10,                // <<, >>, >>>
	PREC_COMPARISON = 9,            // <, <=, >, >=, in, instanceof
	PREC_EQUALITY = 8,              // ==, !=, ===, !==
	PREC_AND = 7,                   // &
	PREC_XOR = 6,                   // ^
	PREC_OR = 5,                    // |
	PREC_LOGICAL_AND = 4,           // &&
	PREC_LOGICAL_OR = 3,            // ||, ??
	PREC_ASSIGNMENT = 2,            // =, +=, -=, **=, *=, /=, %=, <<=, >>=, &=, ^=, |=, &&=, ||=, ??=, =>, ..., yield
	PREC_COMMA = 1,                 // ,
	PREC_NONE = 0,
};

class Parser;
struct ParseRule
{
	std::function<std::shared_ptr<Expr>(Parser *)> prefix;
	std::function<std::shared_ptr<Expr>(Parser *, std::shared_ptr<Expr>)> infix;
	Precedence precedence = PREC_NONE;
};

class Parser
{
public:
	static std::vector<std::shared_ptr<Stmt>> parse(const std::string &);

	Token current;
	Token previous;

	// parse expressions
	std::shared_ptr<Expr> expression(bool required = true);
	std::shared_ptr<Expr> array();
	std::shared_ptr<Expr> arrow();
	std::shared_ptr<Expr> assign(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> binary(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> call(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> comma(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> dot(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> function();
	std::shared_ptr<Expr> grouping();
	std::shared_ptr<Expr> literal();
	std::shared_ptr<Expr> logical(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> new_instance();
	std::shared_ptr<Expr> number();
	std::shared_ptr<Expr> object();
	std::shared_ptr<Expr> string();
	std::shared_ptr<Expr> subscript(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> ternary(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> unary();
	std::shared_ptr<Expr> update(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> variable();

private:
	Parser(const std::string &);

	std::vector<std::shared_ptr<Stmt>> parse_impl();

	// parse statements
	std::shared_ptr<Stmt> statement();
	std::shared_ptr<Stmt> block_stmt();
	std::shared_ptr<Stmt> break_statement();
	std::shared_ptr<Stmt> continue_statement();
	std::shared_ptr<Stmt> do_while_statement();
	std::shared_ptr<Stmt> expression_statement();
	std::shared_ptr<Stmt> if_statement();
	std::shared_ptr<Stmt> return_statement();
	std::shared_ptr<Stmt> for_statement();
	std::shared_ptr<Stmt> throw_statement();
	std::shared_ptr<Stmt> try_statement();
	std::shared_ptr<Stmt> while_statement();

	// parse declarations
	std::shared_ptr<Stmt> declaration();
	std::shared_ptr<Stmt> function_declaration();
	std::shared_ptr<Stmt> var_declaration();

	std::shared_ptr<Expr> parse_precedence(Precedence);
	ParseRule get_rule(TokenType);
	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);
	bool match_any(std::initializer_list<TokenType> const &);
	TokenType peek();
	inline bool check(TokenType type) { return current.type() == type; }
	bool check_any(std::initializer_list<TokenType> const &);

	std::vector<Token> tokens;
	std::vector<Token>::iterator pos;

	std::vector<Token>::iterator save_state() const;
	void restore_state(const std::vector<Token>::iterator &);

	template<class T, typename... Params> std::shared_ptr<T> make_ast_node(Params &&...params)
	{
		auto node = std::make_shared<T>(std::forward<Params>(params)...);
		node->line = previous.line();
		node->col = previous.col();
		return node;
	}
};
};
