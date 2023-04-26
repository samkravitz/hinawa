#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
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
	std::function<std::shared_ptr<Expr>(Parser *)> prefix;
	std::function<std::shared_ptr<Expr>(Parser *, std::shared_ptr<Expr>)> infix;
	Precedence precedence;
};

class Parser
{
public:
	Parser(std::string);

	std::vector<std::shared_ptr<Stmt>> parse();

	Scanner scanner;
	Token current;
	Token previous;

	// parse expressions
	std::shared_ptr<Expr> expression();
	std::shared_ptr<Expr> anonymous();
	std::shared_ptr<Expr> array();
	std::shared_ptr<Expr> assign(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> binary(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> call(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> dot(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> grouping();
	std::shared_ptr<Expr> literal();
	std::shared_ptr<Expr> new_instance();
	std::shared_ptr<Expr> number();
	std::shared_ptr<Expr> object();
	std::shared_ptr<Expr> string();
	std::shared_ptr<Expr> subscript(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> unary();
	std::shared_ptr<Expr> update(std::shared_ptr<Expr>);
	std::shared_ptr<Expr> variable();

private:
	// parse statements
	std::shared_ptr<Stmt> statement();
	std::shared_ptr<Stmt> block_stmt();
	std::shared_ptr<Stmt> expression_statement();
	std::shared_ptr<Stmt> if_statement();
	std::shared_ptr<Stmt> return_statement();
	std::shared_ptr<Stmt> for_statement();
	std::shared_ptr<Stmt> throw_statement();
	std::shared_ptr<Stmt> try_statement();

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

	template<class T, typename... Params> std::shared_ptr<T> make_ast_node(Params &&...params)
	{
		auto node = std::make_shared<T>(std::forward<Params>(params)...);
		node->line = previous.line();
		node->col = previous.col();
		return node;
	}
};
};
