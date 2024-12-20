#include "parser.h"

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "scanner.h"

namespace js
{
Parser::Parser(const std::string &input)
{
	Scanner scanner(input.c_str());
	tokens = scanner.scan();
	pos = tokens.begin();
	current = *pos;
}

ParseRule Parser::get_rule(TokenType type)
{
	static std::unordered_map<TokenType, ParseRule> rules = {
	    {LEFT_PAREN,        {&Parser::grouping, &Parser::call, PREC_NEW}   },
	    {LEFT_BRACKET,      {&Parser::array, &Parser::subscript, PREC_CALL}},
	    {LEFT_BRACE,        {&Parser::object, nullptr, PREC_CALL}          },
	    {COMMA,             {nullptr, &Parser::comma, PREC_COMMA}          },
	    {DOT,	           {nullptr, &Parser::dot, PREC_CALL}             },
	    {MINUS,             {&Parser::unary, &Parser::binary, PREC_TERM}   },
	    {PLUS,              {nullptr, &Parser::binary, PREC_TERM}          },
	    {SLASH,             {nullptr, &Parser::binary, PREC_FACTOR}        },
	    {STAR,              {nullptr, &Parser::binary, PREC_FACTOR}        },
	    {MOD,	           {nullptr, &Parser::binary, PREC_FACTOR}        },
	    {BANG,              {&Parser::unary, nullptr, PREC_NONE}           },
	    {EQUAL,             {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {GREATER,           {nullptr, &Parser::binary, PREC_COMPARISON}    },
	    {LESS,              {nullptr, &Parser::binary, PREC_COMPARISON}    },
	    {AND,	           {nullptr, &Parser::binary, PREC_TERM}          },
	    {PIPE,              {nullptr, &Parser::binary, PREC_TERM}          },
	    {TILDE,             {nullptr, &Parser::binary, PREC_TERM}          },
	    {CARET,             {nullptr, &Parser::binary, PREC_TERM}          },
	    {QUESTION,          {nullptr, &Parser::ternary, PREC_ASSIGNMENT}   },
	    {BANG_EQUAL,        {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {EQUAL_EQUAL,       {nullptr, &Parser::binary, PREC_COMPARISON}    },
	    {GREATER_EQUAL,     {nullptr, &Parser::binary, PREC_COMPARISON}    },
	    {LESS_EQUAL,        {nullptr, &Parser::binary, PREC_COMPARISON}    },
	    {PLUS_EQUAL,        {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {MINUS_EQUAL,       {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {STAR_EQUAL,        {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {SLASH_EQUAL,       {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {AND_EQUAL,         {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {PIPE_EQUAL,        {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {CARET_EQUAL,       {nullptr, &Parser::assign, PREC_ASSIGNMENT}    },
	    {AND_AND,           {nullptr, &Parser::logical, PREC_AND}          },
	    {PIPE_PIPE,         {nullptr, &Parser::logical, PREC_OR}           },
	    {ARROW,             {nullptr, &Parser::binary, PREC_AND}           },
	    {QUESTION_QUESTION, {nullptr, &Parser::binary, PREC_AND}           },
	    {STAR_STAR,         {nullptr, &Parser::binary, PREC_AND}           },
	    {PLUS_PLUS,         {&Parser::unary, &Parser::update, PREC_UNARY}  },
	    {MINUS_MINUS,       {&Parser::unary, &Parser::update, PREC_UNARY}  },
	    {QUESTION_DOT,      {nullptr, &Parser::binary, PREC_AND}           },
	    {EQUAL_EQUAL_EQUAL, {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {BANG_EQUAL_EQUAL,  {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {STAR_STAR_EQUAL,   {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {LESS_LESS_EQUAL,   {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {RIGHT_RIGHT_EQUAL, {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {AND_AND_EQUAL,     {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {PIPE_PIPE_EQUAL,   {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {RIGHT_RIGHT_RIGHT, {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {DOT_DOT_DOT,       {nullptr, &Parser::binary, PREC_EQUALITY}      },
	    {IDENTIFIER,        {&Parser::variable, nullptr, PREC_NONE}        },
	    {STRING,            {&Parser::string, nullptr, PREC_NONE}          },
	    {ESCAPED_STRING,    {&Parser::string, nullptr, PREC_NONE}          },
	    {NUMBER,            {&Parser::number, nullptr, PREC_NONE}          },
	    {HEX_NUMBER,        {&Parser::number, nullptr, PREC_NONE}          },
	    {BIGINT,            {&Parser::number, nullptr, PREC_NONE}          },
	    {KEY_FALSE,         {&Parser::literal, nullptr, PREC_NONE}         },
	    {KEY_FUNCTION,      {&Parser::function, nullptr, PREC_NONE}        },
	    {KEY_INSTANCEOF,    {nullptr, &Parser::binary, PREC_COMPARISON}    },
	    {KEY_NEW,           {&Parser::new_instance, nullptr, PREC_NEW}     },
	    {KEY_NULL,          {&Parser::literal, nullptr, PREC_NONE}         },
	    {KEY_THIS,          {&Parser::variable, nullptr, PREC_NONE}        },
	    {KEY_TRUE,          {&Parser::literal, nullptr, PREC_NONE}         },
	    {KEY_TYPEOF,        {&Parser::unary, nullptr, PREC_UNARY}          },
	    {KEY_UNDEFINED,     {&Parser::literal, nullptr, PREC_NONE}         },
	};

	if (!rules.contains(type))
		return {};

	return rules[type];
}

std::vector<std::shared_ptr<Stmt>> Parser::parse(const std::string &input)
{
	Parser parser(input);
	return parser.parse_impl();
}

std::vector<std::shared_ptr<Stmt>> Parser::parse_impl()
{
	std::vector<std::shared_ptr<Stmt>> program;

	while (!match(TOKEN_EOF))
	{
		auto stmt = declaration();
		if (!stmt)
			break;
		program.push_back(stmt);
	}

	return program;
}

std::shared_ptr<Stmt> Parser::declaration()
{
	if (check_any({KEY_VAR, KEY_LET, KEY_CONST}))
		return var_declaration();

	//if (match(KEY_CLASS))
	//	return class_declaration();

	if (match(KEY_FUNCTION))
		return function_declaration();

	return statement();
}

std::shared_ptr<Stmt> Parser::statement()
{
	if (match(LEFT_BRACE))
	{
		auto block = block_stmt();
		return make_ast_node<ScopeNode>(block);
	}

	if (match(KEY_IF))
		return if_statement();

	if (match(KEY_RETURN))
		return return_statement();

	if (match(KEY_FOR))
		return for_statement();

	if (match(KEY_THROW))
		return throw_statement();

	if (match(KEY_TRY))
		return try_statement();

	if (match(KEY_CONTINUE))
		return continue_statement();

	if (match(KEY_DO))
		return do_while_statement();

	if (match(KEY_WHILE))
		return while_statement();

	if (match(KEY_BREAK))
		return break_statement();

	if (match(KEY_DEBUGGER))
	{
		// match optional semicolon after break statement
		match(SEMICOLON);
		return make_ast_node<DebuggerStmt>();
	}

	return expression_statement();
}

std::shared_ptr<Stmt> Parser::block_stmt()
{
	std::vector<std::shared_ptr<Stmt>> stmts;

	while (!match(RIGHT_BRACE))
		stmts.push_back(declaration());

	return make_ast_node<BlockStmt>(stmts);
}

std::shared_ptr<Stmt> Parser::break_statement()
{
	// match optional semicolon after break statement
	match(SEMICOLON);
	return make_ast_node<BreakStmt>();
}

std::shared_ptr<Stmt> Parser::continue_statement()
{
	// match optional semicolon after continue statement
	match(SEMICOLON);
	return make_ast_node<ContinueStmt>();
}

std::shared_ptr<Stmt> Parser::var_declaration()
{
	std::vector<VarDecl::VarDeclarator> declorators;
	std::shared_ptr<Expr> initializer = nullptr;
	VarDecl::VarDeclKind kind;

	if (match(KEY_VAR))
		kind = VarDecl::VAR;
	else if (match(KEY_CONST))
		kind = VarDecl::CONST;
	else if (match(KEY_LET))
		kind = VarDecl::LET;
	else
		assert(!"Unknown var decl type!");

	do
	{
		consume(IDENTIFIER, "Expected variable name");

		auto identifier = previous.value();
		initializer = nullptr;

		if (match(EQUAL))
			initializer = expression();

		declorators.push_back({identifier, initializer});

	} while (match(COMMA));

	// match optional semicolon after expression statement
	match(SEMICOLON);

	return make_ast_node<VarDecl>(declorators, kind);
}

std::shared_ptr<Stmt> Parser::expression_statement()
{
	std::shared_ptr<Stmt> stmt = nullptr;
	std::shared_ptr<Expr> expr;
	if ((expr = expression()))
	{
		// match optional semicolon after expression statement
		match(SEMICOLON);
		stmt = make_ast_node<ExpressionStmt>(expr);
	}

	return stmt;
}

std::shared_ptr<Stmt> Parser::if_statement()
{
	consume(LEFT_PAREN, "Expected '('");
	std::shared_ptr<Expr> test = expression();
	consume(RIGHT_PAREN, "Expected ')'");

	std::shared_ptr<Stmt> consequence = statement();
	std::shared_ptr<Stmt> alternate = nullptr;

	if (match(KEY_ELSE))
		alternate = statement();

	return make_ast_node<IfStmt>(test, consequence, alternate);
}

std::shared_ptr<Stmt> Parser::return_statement()
{
	auto expr = expression(false);

	// match optional semicolon after return statement
	match(SEMICOLON);

	return make_ast_node<ReturnStmt>(expr);
}

std::shared_ptr<Stmt> Parser::for_statement()
{
	consume(LEFT_PAREN, "Expected '('");

	std::shared_ptr<AstNode> initialization = nullptr;
	if (check_any({KEY_VAR, KEY_LET, KEY_CONST}))
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
		initialization = expression(false);
		consume(SEMICOLON, "For statement: expect ; after initializer");
	}

	auto condition = expression();
	consume(SEMICOLON, "For statement: expect ; after condition");
	auto afterthought = expression();
	consume(RIGHT_PAREN, "Expected ')'");

	auto stmt = statement();

	return make_ast_node<ForStmt>(initialization, condition, afterthought, stmt);
}

std::shared_ptr<Stmt> Parser::do_while_statement()
{
	auto stmt = statement();
	consume(KEY_WHILE, "Expected 'while' in do...while loop");
	consume(LEFT_PAREN, "Expected '('");
	auto condition = expression();
	consume(RIGHT_PAREN, "Expected ')'");

	// match optional semicolon after while statement
	match(SEMICOLON);

	return make_ast_node<WhileStmt>(condition, stmt);
}

std::shared_ptr<Stmt> Parser::while_statement()
{
	consume(LEFT_PAREN, "Expected '('");
	auto condition = expression();
	consume(RIGHT_PAREN, "Expected ')'");
	auto stmt = statement();

	return make_ast_node<WhileStmt>(condition, stmt);
}

std::shared_ptr<Stmt> Parser::throw_statement()
{
	auto expr = expression();

	// match optional semicolon after throw statement
	match(SEMICOLON);
	return make_ast_node<ThrowStmt>(expr);
}

std::shared_ptr<Stmt> Parser::try_statement()
{
	std::shared_ptr<Stmt> block = nullptr;
	std::shared_ptr<Stmt> handler = nullptr;
	std::shared_ptr<Stmt> finalizer = nullptr;
	std::optional<std::string> catch_param = {};

	consume(LEFT_BRACE, "Expect '{' after try");
	block = block_stmt();

	if (match(KEY_CATCH))
	{
		if (match(LEFT_PAREN))
		{
			consume(IDENTIFIER, "Expect identifier name");
			catch_param = previous.value();
			consume(RIGHT_PAREN, "Expect ')'");
		}

		consume(LEFT_BRACE, "Expect '{' after catch");
		handler = block_stmt();
	}

	if (match(KEY_FINALLY))
	{
		consume(LEFT_BRACE, "Expect '{' after finally");
		finalizer = block_stmt();
	}

	if (!handler && !finalizer)
	{
		std::cerr << "Error: try needs either catch or finally\n";
	}

	return make_ast_node<TryStmt>(std::static_pointer_cast<BlockStmt>(block),
	                              std::static_pointer_cast<BlockStmt>(handler),
	                              std::static_pointer_cast<BlockStmt>(finalizer),
	                              catch_param);
}

std::shared_ptr<Stmt> Parser::function_declaration()
{
	consume(IDENTIFIER, "Expected identifier");
	auto name = previous.value();
	consume(LEFT_PAREN, "Expected '('");

	std::vector<std::string> args;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			if (args.size() > 0xff)
				std::cerr << "Can't have more than 255 parameters\n";

			consume(IDENTIFIER, "Expect parameter name");
			args.push_back(previous.value());
		} while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expected ')'");
	consume(LEFT_BRACE, "Expected '{'");

	auto block = block_stmt();

	return make_ast_node<FunctionDecl>(name, args, std::static_pointer_cast<BlockStmt>(block));
}

std::shared_ptr<Expr> Parser::expression(bool required)
{
	auto expr = parse_precedence(PREC_ASSIGNMENT);
	if (!expr && required)
		fmt::print(stderr, "[line {}]: Expect expression\n", current.line());

	return expr;
}

std::shared_ptr<Expr> Parser::array()
{
	std::vector<std::shared_ptr<Expr>> elements;
	if (!check(RIGHT_BRACKET))
	{
		for (;;)
		{
			if (elements.size() > 0xff)
				fmt::print(stderr, "Can't have more than 255 elements in an array expression\n");

			auto expr = expression();
			if (!expr)
				fmt::print(stderr, "Expected expression\n");

			elements.push_back(expr);

			if (match(COMMA))
			{
				if (check(RIGHT_BRACKET))
					break;
			}
			else
			{
				if (!check(RIGHT_BRACKET))
					fmt::print(stderr, "Invalid array literal");
				else
					break;
			}
		}
	}
	consume(RIGHT_BRACKET, "Expect ']' after array expression");

	return make_ast_node<ArrayExpr>(elements);
}

std::shared_ptr<Expr> Parser::arrow()
{
	std::vector<std::string> params;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			if (params.size() > 0xff)
				std::cerr << "Can't have more than 255 parameters\n";

			consume(IDENTIFIER, "Expect parameter name");
			params.push_back(previous.value());
		} while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expected ')'");
	consume(ARROW, "Expected '=>'");
	consume(LEFT_BRACE, "Expected '{'");
	auto body = block_stmt();

	return make_ast_node<FunctionExpr>("", params, std::static_pointer_cast<BlockStmt>(body));
}

std::shared_ptr<Expr> Parser::assign(std::shared_ptr<Expr> left)
{
	auto op = previous;
	auto right = expression();

	static std::unordered_map<std::string, TokenType> compound_assignment_ops = {
	    {"+=", TokenType::PLUS },
	    {"-=", TokenType::MINUS},
	    {"*=", TokenType::STAR },
	    {"/=", TokenType::SLASH},
	};

	if (compound_assignment_ops.contains(op.value()))
	{
		auto assignment_token = Token(
		    op.value().substr(0, op.value().size() - 1), compound_assignment_ops[op.value()], op.line(), op.col());
		right = make_ast_node<BinaryExpr>(left, assignment_token, right);
		op = Token("=", TokenType::EQUAL, op.line(), op.col());
	}

	return make_ast_node<AssignmentExpr>(left, op, right);
}

std::shared_ptr<Expr> Parser::binary(std::shared_ptr<Expr> left)
{
	auto op = previous;
	auto precedence = get_rule(op.type()).precedence;
	auto right = parse_precedence(static_cast<Precedence>(precedence + 1));
	return make_ast_node<BinaryExpr>(left, op, right);
}

std::shared_ptr<Expr> Parser::call(std::shared_ptr<Expr> left)
{
	std::vector<std::shared_ptr<Expr>> args;
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
	return make_ast_node<CallExpr>(left, args);
}

std::shared_ptr<Expr> Parser::comma(std::shared_ptr<Expr> left)
{
	return nullptr;
}

std::shared_ptr<Expr> Parser::dot(std::shared_ptr<Expr> left)
{
	consume(IDENTIFIER, "Expect identifier after '.'");
	return make_ast_node<MemberExpr>(left, make_ast_node<Variable>(previous.value()), true);
}

std::shared_ptr<Expr> Parser::function()
{
	std::string name = "";
	if (match(IDENTIFIER))
		name = previous.value();

	consume(LEFT_PAREN, "Expected '('");

	std::vector<std::string> args;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			if (args.size() > 0xff)
				std::cerr << "Can't have more than 255 arguments\n";

			consume(IDENTIFIER, "Expect arguments name");
			args.push_back(previous.value());
		} while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expected ')'");
	consume(LEFT_BRACE, "Expected '{'");
	auto body = block_stmt();

	return make_ast_node<FunctionExpr>(name, args, std::static_pointer_cast<BlockStmt>(body));
}

std::shared_ptr<Expr> Parser::grouping()
{
	auto state = save_state();
	auto expr = parse_precedence(PREC_COMMA);

	match(RIGHT_PAREN);

	if (!expr || check(ARROW))
	{
		restore_state(state);
		return arrow();
	}

	return expr;
}

std::shared_ptr<Expr> Parser::literal()
{
	return make_ast_node<Literal>(previous);
}

std::shared_ptr<Expr> Parser::logical(std::shared_ptr<Expr> left)
{
	auto op = previous;
	auto precedence = get_rule(op.type()).precedence;
	auto right = parse_precedence(static_cast<Precedence>(precedence + 1));
	return make_ast_node<LogicalExpr>(left, op, right);
}

std::shared_ptr<Expr> Parser::new_instance()
{
	auto callee = parse_precedence(PREC_GROUPING);
	std::vector<std::shared_ptr<Expr>> args;
	if (match(LEFT_PAREN))
	{
		if (!check(RIGHT_PAREN))
		{
			do
			{
				if (args.size() > 0xff)
					std::cerr << "Can't have more than 255 arguments\n";

				args.push_back(expression());
			} while (match(COMMA));
		}
		consume(RIGHT_PAREN, "Expect '}'");
	}

	return make_ast_node<NewExpr>(callee, args);
}

std::shared_ptr<Expr> Parser::number()
{
	return make_ast_node<Literal>(previous);
}

std::shared_ptr<Expr> Parser::object()
{
	std::vector<std::pair<std::string, std::shared_ptr<Expr>>> properties;
	if (!check(RIGHT_BRACE))
	{
		for (;;)
		{
			if (properties.size() > 0xff)
				std::cerr << "Can't have more than 255 properties in an object literal\n";

			consume(IDENTIFIER, "Expected object key name");
			auto ident = previous.value();
			consume(COLON, "Expect : after object key name");
			auto expr = expression();
			properties.push_back({ident, expr});

			if (match(COMMA))
			{
				if (check(RIGHT_BRACE))
					break;
			}
			else
			{
				if (!check(RIGHT_BRACE))
					fmt::print(stderr, "Invalid object literal");
				else
					break;
			}
		}
	}

	consume(RIGHT_BRACE, "Expect '}' after object literal");

	return make_ast_node<ObjectExpr>(properties);
}

std::shared_ptr<Expr> Parser::string()
{
	auto string_token = previous;
	if (string_token.type() == ESCAPED_STRING)
	{
		std::string adjusted_string = "";
		auto str = string_token.value();
		for (auto it = str.begin(); it != str.end();)
		{
			if (*it == '\\')
			{
				it += 2;
				auto escaped_value = std::string(it, it + 2);
				unsigned char c = std::stol(escaped_value, nullptr, 16);
				adjusted_string += c;
				it += 2;
			}
			else
			{
				adjusted_string += *it;
				it++;
			}
		}
		string_token = Token(adjusted_string, STRING, string_token.line(), string_token.col());
	}

	return make_ast_node<Literal>(string_token);
}

std::shared_ptr<Expr> Parser::subscript(std::shared_ptr<Expr> left)
{
	auto expr = expression();
	consume(RIGHT_BRACKET, "Expect ']' after subscript");
	return make_ast_node<MemberExpr>(left, expr);
}

std::shared_ptr<Expr> Parser::ternary(std::shared_ptr<Expr> left)
{
	auto if_true = expression();
	consume(COLON, "Expect ':' after first expression in ternary");
	auto if_false = expression();
	return make_ast_node<TernaryExpr>(left, if_true, if_false);
}

std::shared_ptr<Expr> Parser::unary()
{
	auto op = previous;
	auto operand = parse_precedence(PREC_UNARY);
	if (op.type() == PLUS_PLUS || op.type() == MINUS_MINUS)
		return make_ast_node<UpdateExpr>(op, operand, true);

	return make_ast_node<UnaryExpr>(op, operand);
}

std::shared_ptr<Expr> Parser::update(std::shared_ptr<Expr> left)
{
	auto op = previous;
	return make_ast_node<UpdateExpr>(op, left, false);
}

std::shared_ptr<Expr> Parser::variable()
{
	return make_ast_node<Variable>(previous.value());
}

/**
* @brief parses an expression at a given precedence level or higher
* @param precedence the lowest precedence level to parse
* @return the expression, or nullptr if there are none
*/
std::shared_ptr<Expr> Parser::parse_precedence(Precedence precedence)
{
	advance();
	auto prefix = get_rule(previous.type()).prefix;
	if (!prefix)
		return nullptr;

	std::shared_ptr<Expr> expr = prefix(this);

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

	if (pos == tokens.end() - 1)
		current = {};
	else
		current = *++pos;
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
	for (const auto &t : tokens)
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
		fmt::print(stderr, "{}\n", msg);
}

TokenType Parser::peek()
{
	return current.type();
}

std::vector<Token>::iterator Parser::save_state() const
{
	return pos;
}

void Parser::restore_state(const std::vector<Token>::iterator &saved_pos)
{
	pos = saved_pos;
	current = *pos;
	previous = *(pos - 1);
}
}
