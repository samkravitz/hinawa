#include "compiler.h"

#include <cassert>
#include <functional>

#include <fmt/format.h>

namespace js
{
struct ParseRule
{
	std::function<void(Compiler *, bool)> prefix;
	std::function<void(Compiler *, bool)> infix;
	Precedence precedence;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
ParseRule rules[] = {
	[0]                 = { nullptr, nullptr, PREC_NONE },
	[LEFT_PAREN]        = { &Compiler::grouping, &Compiler::call, PREC_CALL },
	[RIGHT_PAREN]       = { nullptr, nullptr, PREC_NONE },
	[LEFT_BRACE]        = { &Compiler::object, nullptr, PREC_SUBSCRIPT },
	[RIGHT_BRACE]       = { nullptr, nullptr, PREC_NONE },
	[LEFT_BRACKET]      = { &Compiler::array, &Compiler::subscript, PREC_SUBSCRIPT },
	[RIGHT_BRACKET]     = { nullptr, nullptr, PREC_NONE },
	[COMMA]             = { nullptr, nullptr, PREC_NONE },
	[DOT]               = { nullptr, &Compiler::dot, PREC_CALL },
	[MINUS]             = { &Compiler::unary, &Compiler::binary, PREC_TERM },
	[PLUS]              = { nullptr, &Compiler::binary, PREC_TERM },
	[SLASH]             = { nullptr, &Compiler::binary, PREC_FACTOR },
	[STAR]              = { nullptr, &Compiler::binary, PREC_FACTOR },
	[MOD]               = { nullptr, &Compiler::binary, PREC_FACTOR },
	[SEMICOLON]         = { nullptr, nullptr, PREC_NONE },
	[COLON]             = { nullptr, nullptr, PREC_NONE },
	[BANG]              = { nullptr, nullptr, PREC_NONE },
	[EQUAL]             = { nullptr, nullptr, PREC_NONE },
	[GREATER]           = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[LESS]              = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[AND]               = { nullptr, &Compiler::binary, PREC_TERM },
	[PIPE]              = { nullptr, &Compiler::binary, PREC_TERM },
	[TILDE]             = { nullptr, &Compiler::binary, PREC_TERM },
	[CARET]             = { nullptr, &Compiler::binary, PREC_TERM },
	[QUESTION]          = { nullptr, &Compiler::binary, PREC_TERM },
	[NEWLINE]           = { nullptr, nullptr, PREC_NONE },

	[BANG_EQUAL]        = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[EQUAL_EQUAL]       = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[GREATER_EQUAL]     = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[LESS_EQUAL]        = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[PLUS_EQUAL]        = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[MINUS_EQUAL]       = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[STAR_EQUAL]        = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[SLASH_EQUAL]       = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[AND_EQUAL]         = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[PIPE_EQUAL]        = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[CARET_EQUAL]       = { nullptr, &Compiler::binary, PREC_COMPARISON },
	[LESS_LESS]         = { nullptr, nullptr, PREC_NONE },
	[GREATER_GREATER]   = { nullptr, nullptr, PREC_NONE },
	[AND_AND]           = { nullptr, &Compiler::binary, PREC_AND },
	[PIPE_PIPE]         = { nullptr, &Compiler::binary, PREC_OR },
	[ARROW]             = { nullptr, &Compiler::binary, PREC_AND },
	[QUESTION_QUESTION] = { nullptr, &Compiler::binary, PREC_AND },
	[STAR_STAR]         = { nullptr, &Compiler::binary, PREC_AND },
	[PLUS_PLUS]         = { nullptr, &Compiler::binary, PREC_AND },
	[MINUS_MINUS]       = { nullptr, &Compiler::binary, PREC_AND },
	[QUESTION_DOT]      = { nullptr, &Compiler::binary, PREC_AND },

	[EQUAL_EQUAL_EQUAL] = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[BANG_EQUAL_EQUAL]  = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[STAR_STAR_EQUAL]   = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[LESS_LESS_EQUAL]   = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[RIGHT_RIGHT_EQUAL] = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[AND_AND_EQUAL]     = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[PIPE_PIPE_EQUAL]   = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[RIGHT_RIGHT_RIGHT] = { nullptr, &Compiler::binary, PREC_EQUALITY },
	[DOT_DOT_DOT]       = { nullptr, &Compiler::binary, PREC_EQUALITY },

	[IDENTIFIER]        = { &Compiler::variable, nullptr, PREC_NONE },
	[STRING]            = { &Compiler::string, nullptr, PREC_NONE },
	[NUMBER]            = { &Compiler::number, nullptr, PREC_NONE },

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
	[KEY_FALSE]         = { &Compiler::literal, nullptr, PREC_NONE },
	[KEY_FINALLY]       = { nullptr, nullptr, PREC_NONE },
	[KEY_FOR]           = { nullptr, nullptr, PREC_NONE },
	[KEY_FUNCTION]      = { &Compiler::anonymous, nullptr, PREC_NONE },
	[KEY_IF]            = { nullptr, nullptr, PREC_NONE },
	[KEY_IMPLEMENTS]    = { nullptr, nullptr, PREC_NONE },
	[KEY_IMPORT]        = { nullptr, nullptr, PREC_NONE },
	[KEY_IN]            = { nullptr, nullptr, PREC_NONE },
	[KEY_INSTANCEOF]    = { nullptr, nullptr, PREC_NONE },
	[KEY_INTERFACE]     = { nullptr, nullptr, PREC_NONE },
	[KEY_LET]           = { nullptr, nullptr, PREC_NONE },
	[KEY_NEW]           = { &Compiler::new_instance, nullptr, PREC_NONE },
	[KEY_NULL]          = { &Compiler::literal, nullptr, PREC_NONE },
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
	[KEY_TRUE]          = { &Compiler::literal, nullptr, PREC_NONE },
	[KEY_TYPEOF]        = { nullptr, nullptr, PREC_NONE },
	[KEY_UNDEFINED]     = { &Compiler::literal, nullptr, PREC_NONE },
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

Compiler::Compiler(const char *src) :
    scanner(src)
{ }

void Compiler::init_compiler(FunctionCompiler *compiler)
{
	compiler->enclosing = current;
	current = compiler;

	current->locals.push_back({ "", 0 });
}

void Compiler::end_compiler()
{
	if (current->function->chunk.code.empty())
		emit_constant(Value(nullptr));
	
	emit_byte(OP_RETURN);

	auto function = *current->function;
	#ifdef DEBUG_PRINT_CODE
	const char *name = function.type == ANONYMOUS ? "anonymous" : function.name.c_str();
	function.chunk.disassemble(name);
	#endif

	current = current->enclosing;
}

Function Compiler::compile()
{
	Function script("script", SCRIPT);
	FunctionCompiler compiler(current, &script);
	init_compiler(&compiler);

	advance();

	while (!match(TOKEN_EOF))
		declaration();

	end_compiler();
	return script;
}

void Compiler::advance()
{
	parser.previous = parser.current;
	parser.current = scanner.next();
}

void Compiler::consume(TokenType type, const char *msg)
{
	if (parser.current.type() != type)
		error_at_current(msg);

	advance();
}

bool Compiler::match(TokenType type)
{
	if (parser.current.type() == type)
	{
		advance();
		return true;
	}

	return false;
}

bool Compiler::check(TokenType type)
{
	return parser.current.type() == type;
}

void Compiler::statement()
{
	if (match(KEY_IF))
	{
		if_statement();
		return;
	}

	if (match(KEY_RETURN))
	{
		return_statement();
		return;
	}

	if (match(KEY_TRY))
	{
		try_statement();
		return;
	}

	if (match(KEY_WHILE))
	{
		while_statement();
		return;
	}

	if (match(KEY_THROW))
	{
		throw_statement();
		return;
	}

	if (match(LEFT_BRACE))
	{
		begin_scope();
		block();
		end_scope();
		return;
	}

	expression_statement();
}

void Compiler::declaration()
{
	if (match(KEY_VAR))
	{
		var_declaration();
		return;
	}

	if (match(KEY_CLASS))
	{
		class_declaration();
		return;
	}

	if (match(KEY_FUNCTION))
	{
		function_declaration();
		return;
	}

	statement();
}

void Compiler::parse_precedence(Precedence precedence)
{
	advance();
	auto prefix = get_rule(parser.previous.type())->prefix;
	if (!prefix)
	{
		error("Expect expression");
		return;
	}

	auto can_assign = precedence <= PREC_ASSIGNMENT;
	prefix(this, can_assign);

	while (precedence <= get_rule(parser.current.type())->precedence)
	{
		advance();
		auto infix = get_rule(parser.previous.type())->infix;
		infix(this, can_assign);

		if (can_assign && match(EQUAL))
			error("Invalid assignment target");
	}
}

void Compiler::anonymous(bool can_assign)
{
	auto function = Function(ANONYMOUS);
	FunctionCompiler compiler(current, &function);
	init_compiler(&compiler);
	begin_scope();
	consume(LEFT_PAREN, "Expect '(' after function");

	int arity = 0;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			arity++;
			if (arity > 0xff)
				error_at_current("Can't have more than 255 parameters");

			auto constant = parse_variable("Expect parameter name");
			define_variable(constant);
		} while (match(COMMA));
	}

	current->function->arity = arity;
	consume(RIGHT_PAREN, "Expect ')' after parameters");
	consume(LEFT_BRACE, "Expect '{' before function body");
	block();

	end_compiler();
	emit_bytes(OP_CONSTANT, make_constant(Value(new Function(function))));
}

void Compiler::array(bool can_assign)
{
	auto num_elements = 0;

	if (!check(RIGHT_BRACKET))
	{
		do
		{
			num_elements += 1;
			if (num_elements > 0xff)
				error("Can't have more than 255 items in an array literal");
			expression();
		} while (match(COMMA));
	}

	consume(RIGHT_BRACKET, "Expect ']' after array literal");
	emit_bytes(OP_NEW_ARRAY, num_elements);
}

void Compiler::object(bool can_assign)
{
	auto num_elements = 0;
	std::vector<u8> key_constants;

	if (!check(RIGHT_BRACE))
	{
		do
		{
			num_elements += 1;
			if (num_elements > 0xff)
				error("Can't have more than 255 properties in an object literal");

			auto constant = parse_variable("Expected object key name");
			key_constants.push_back(constant);
			consume(COLON, "Expect : after object key name");
			expression();
		} while (match(COMMA));
	}

	consume(RIGHT_BRACE, "Expect '}' after object literal");
	emit_bytes(OP_NEW_OBJECT, num_elements);
	for (auto x : key_constants)
		emit_byte(x);
}

void Compiler::binary(bool can_assign)
{
	auto op = parser.previous.type();
	auto precedence = get_rule(op)->precedence;
	parse_precedence(precedence);

	switch (op)
	{
		case PLUS:
			emit_byte(OP_ADD);
			break;
		case MINUS:
			emit_byte(OP_SUBTRACT);
			break;
		case STAR:
			emit_byte(OP_MULTIPLY);
			break;
		case SLASH:
			emit_byte(OP_DIVIDE);
			break;
		case MOD:
			emit_byte(OP_MOD);
			break;
		case BANG_EQUAL:
			emit_bytes(OP_EQUAL, OP_NOT);
			break;
		case EQUAL_EQUAL:
			emit_byte(OP_EQUAL);
			break;
		case EQUAL_EQUAL_EQUAL:
			emit_byte(OP_EQUAL);
			break;
		case GREATER:
			emit_byte(OP_GREATER);
			break;
		case GREATER_EQUAL:
			emit_bytes(OP_LESS, OP_NOT);
			break;
		case LESS:
			emit_byte(OP_LESS);
			break;
		case LESS_EQUAL:
			emit_bytes(OP_GREATER, OP_NOT);
			break;
		case AND:
			emit_byte(OP_BITWISE_AND);
			break;
		case PIPE:
			emit_byte(OP_BITWISE_OR);
			break;
		case AND_AND:
			emit_byte(OP_LOGICAL_AND);
			break;
		case PIPE_PIPE:
			emit_byte(OP_LOGICAL_OR);
			break;
		default:
			fmt::print("Unknown binary op {}\n", parser.previous.value());
	}
}

void Compiler::call(bool can_assign)
{
	int argc = 0;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			expression();
			argc++;
			if (argc > 0xff)
				error("Can't have more than 255 arguments");
		} while (match(COMMA));
	}

	consume(RIGHT_PAREN, "Expect ')' after arguments");
	emit_bytes(OP_CALL, argc);
}

void Compiler::dot(bool can_assign)
{
	consume(IDENTIFIER, "Expect identifier after '.'");
	auto constant = make_constant(Value(new std::string(parser.previous.value())));

	if (can_assign && match(EQUAL))
	{
		expression();
		emit_bytes(OP_SET_PROPERTY, constant);
	}

	else
	{
		emit_bytes(OP_GET_PROPERTY, constant);
	}
}

void Compiler::grouping(bool can_assign)
{
	expression();
	consume(RIGHT_PAREN, "Expect ')' after expression");
}

void Compiler::literal(bool can_assign)
{
	switch (parser.previous.type())
	{
		case KEY_FALSE:
			emit_byte(OP_FALSE);
			break;
		case KEY_NULL:
			emit_byte(OP_NULL);
			break;
		case KEY_TRUE:
			emit_byte(OP_TRUE);
			break;
		case KEY_UNDEFINED:
			emit_byte(OP_UNDEFINED);
			break;
		default:
			assert(!"Unknown literal!");
	}
}

void Compiler::new_instance(bool can_assign)
{
	parse_variable("Expect identifier in new");
	int argc = 0;

	if (match(LEFT_PAREN))
	{
		if (!check(RIGHT_PAREN))
		{
			do
			{
				expression();
				argc++;
				if (argc > 0xff)
					error("Can't have more than 255 arguments");
			} while (match(COMMA));
		}
		consume(RIGHT_PAREN, "Expect ')' after arguments");
	}

	match(SEMICOLON);
	emit_bytes(OP_CALL, argc);
}

void Compiler::number(bool can_assign)
{
	auto d = std::stod(parser.previous.value());
	emit_constant(Value(d));
}

void Compiler::string(bool can_assign)
{
	auto str = parser.previous.value();
	emit_constant(Value(new std::string(str.substr(1, str.size() - 2))));
}

void Compiler::subscript(bool can_assign)
{
	expression();
	consume(RIGHT_BRACKET, "Expect ']' after index");

	if (can_assign && match(EQUAL))
	{
		expression();
		emit_byte(OP_SET_SUBSCRIPT);
	}

	else
	{
		emit_byte(OP_GET_SUBSCRIPT);
	}
}

void Compiler::unary(bool can_assign)
{
	auto op = parser.previous.type();
	parse_precedence(PREC_UNARY);

	switch (op)
	{
		case MINUS: emit_byte(OP_NEGATE); break;
		case BANG: emit_byte(OP_NOT); break;
		default:
			assert(!"Unknown unary operator!");
	}
}

void Compiler::variable(bool can_assign)
{
	auto identifier = parser.previous;

	Opcode get_op, set_op;
	int value = resolve_local(identifier);

	if (value == -1)
	{
		get_op = OP_GET_GLOBAL;
		set_op = OP_SET_GLOBAL;
		value = make_constant(Value(new std::string(identifier.value())));
	}

	else
	{
		get_op = OP_GET_LOCAL;
		set_op = OP_SET_LOCAL;
	}

	if (can_assign && match(EQUAL))
	{
		expression();
		emit_bytes(set_op, value);
	}

	else
	{
		emit_bytes(get_op, value);
	}
}

void Compiler::expression_statement()
{
	expression();
	match(SEMICOLON);
	emit_byte(OP_POP);
}

void Compiler::expression()
{
	parse_precedence(PREC_ASSIGNMENT);
}

void Compiler::block()
{
	while (!check(RIGHT_BRACE))
		declaration();

	consume(RIGHT_BRACE, "Expect '}' after block");
}

void Compiler::if_statement()
{
	consume(LEFT_PAREN, "Expect '(' after if");
	expression();
	consume(RIGHT_PAREN, "Expect ')' after condition");

	auto then_offset = emit_jump(OP_JUMP_IF_FALSE);
	emit_byte(OP_POP);
	statement();

	auto else_offset = emit_jump(OP_JUMP);
	emit_byte(OP_POP);
	patch_jump(then_offset);

	if (match(KEY_ELSE))
		statement();

	patch_jump(else_offset);
}

void Compiler::try_statement()
{
	int catch_jump = emit_jump(OP_PUSH_EXCEPTION);
	consume(LEFT_BRACE, "Expect '{' after try");
	begin_scope();
	block();
	end_scope();

	int finally_jump = emit_jump(OP_JUMP);
	bool has_catch = false;
	bool has_finally = false;

	if (match(KEY_CATCH))
	{
		has_catch = true;
		if (match(LEFT_PAREN))
		{
			auto catch_var = parse_variable("Expect catch variable");
			define_variable(catch_var);
			consume(RIGHT_PAREN, "Expect ')' after catch variable");
		}

		consume(LEFT_BRACE, "Expect '{' after catch");
		begin_scope();
		block();
		end_scope();
	}

	patch_jump(catch_jump);
	emit_byte(OP_POP_EXCEPTION);
	patch_jump(finally_jump);

	if (match(KEY_FINALLY))
	{
		has_finally = true;
		consume(LEFT_BRACE, "Expect '{' after finally");
		begin_scope();
		block();
		end_scope();
	}

	if (!has_catch && !has_finally)
		error("Expected catch or finally");
}

void Compiler::throw_statement()
{
	expression();
	emit_byte(OP_THROW);
}

void Compiler::while_statement()
{
	auto loop_start = current_function().chunk.size();
	expression();
	auto exit_offset = emit_jump(OP_JUMP_IF_FALSE);
	emit_byte(OP_POP);

	consume(LEFT_BRACE, "Expect '{' before while block");
	block();
	emit_loop(loop_start);

	patch_jump(exit_offset);
	emit_byte(OP_POP);
}

void Compiler::return_statement()
{
	// TODO: don't parse expression if return is followed immediately by \n

	if (match(SEMICOLON))
	{
		emit_bytes(OP_UNDEFINED, OP_RETURN);
		return;
	}

	expression();
	match(SEMICOLON);
	emit_byte(OP_RETURN);
}

void Compiler::var_declaration()
{
	auto global = parse_variable("Expect variable name");

	if (match(EQUAL))
		expression();
	else
		emit_byte(OP_UNDEFINED);

	match(SEMICOLON);
	define_variable(global);
}

void Compiler::function_declaration()
{
	auto name = parser.current.value();
	auto global = parse_variable("Expect function name");

	auto function = Function(name);
	FunctionCompiler compiler(current, &function);
	init_compiler(&compiler);

	begin_scope();
	consume(LEFT_PAREN, "Expect '(' after function name");

	int arity = 0;
	if (!check(RIGHT_PAREN))
	{
		do
		{
			arity++;
			if (arity > 0xff)
				error_at_current("Can't have more than 255 parameters");

			auto constant = parse_variable("Expect parameter name");
			define_variable(constant);
		} while (match(COMMA));
	}

	current->function->arity = arity;
	consume(RIGHT_PAREN, "Expect ')' after parameters");
	consume(LEFT_BRACE, "Expect '{' before function body");
	block();

	end_compiler();
	emit_bytes(OP_CONSTANT, make_constant(Value(new Function(function))));
	define_variable(global);
}

void Compiler::class_declaration()
{
	//consume(IDENTIFIER, "Expect class name");
	//auto klass = make_constant(Value(parser.previous.value()));

	//emit_bytes(OP_CLASS, klass);
	//emit_bytes(OP_SET_GLOBAL, klass);

	//consume(LEFT_BRACE, "Expect '{' before class body");
	//consume(RIGHT_BRACE, "Expect '}' after class body");
}

size_t Compiler::make_constant(Value value)
{
	auto constant = current_function().chunk.add_constant(value);
	if (constant > 0xff)
	{
		error("Too many constants in this chunk");
		return 0;
	}

	return constant;
}

void Compiler::emit_byte(u8 byte)
{
	current_function().chunk.write(byte, parser.previous.line());
}

void Compiler::emit_bytes(u8 a, u8 b)
{
	emit_byte(a);
	emit_byte(b);
}

void Compiler::emit_constant(Value value)
{
	auto constant = make_constant(value);
	emit_bytes(OP_CONSTANT, constant);
}

size_t Compiler::emit_jump(Opcode op)
{
	emit_byte(op);
	emit_bytes(0xff, 0xff);
	return current_function().chunk.size() - 2;
}

void Compiler::patch_jump(size_t offset)
{
	auto jump = current_function().chunk.size() - offset - 2;
	if (jump > 0xffff)
		error("Jump is out of bounds");

	current_function().chunk.code[offset] = (jump >> 8) & 0xff;
	current_function().chunk.code[offset + 1] = jump & 0xff;
}

void Compiler::emit_loop(size_t loop_start)
{
	emit_byte(OP_LOOP);
	auto offset = current_function().chunk.size() - loop_start + 2;
	if (offset > 0xffff)
		error("Loop offset is out of bounds");

	emit_byte((offset >> 8) & 0xff);
	emit_byte(offset & 0xff);
}

void Compiler::error(const char *msg)
{
	error_at(parser.previous, msg);
}

void Compiler::error_at_current(const char *msg)
{
	error_at(parser.current, msg);
}

void Compiler::error_at(Token t, const char *msg)
{
	std::cout << "[line " << t.line() << "] Error: " << msg << "\n";
}

void Compiler::add_local(Token t)
{
	auto local = Local{ t.value(), -1 };
	current->locals.push_back(local);
}

u8 Compiler::parse_variable(const char *msg)
{
	consume(IDENTIFIER, msg);
	declare_variable();
	if (!is_global())
		return 0;

	return identifier_constant(parser.previous);
}

void Compiler::define_variable(u8 global)
{
	if (!is_global())
	{
		mark_initialized();
		return;
	}

	emit_bytes(OP_DEFINE_GLOBAL, global);
}

void Compiler::declare_variable()
{
	if (is_global())
		return;

	auto t = parser.previous;

	auto local_count = current->local_count();
	for (int i = local_count - 1; i >= 0; i--)
	{
		auto local = current->locals[i];
		if (local.depth != -1 && local.depth < current->scope_depth)
			break;

		if (t.value() == local.name)
			error("Already a variable with this name in this scope.");
	}

	add_local(t);
}

u8 Compiler::identifier_constant(Token const &name)
{
	return make_constant(Value(new std::string(name.value())));
}

void Compiler::mark_initialized()
{
	if (is_global())
		return;

	current->locals[current->local_count() - 1].depth = current->scope_depth;
}

int Compiler::resolve_local(Token t)
{
	auto local_count = current->local_count();
	for (int i = local_count - 1; i >= 0; i--)
	{
		auto local = current->locals[i];
		if (t.value() == local.name)
			return i;
	}

	return -1;
}

void Compiler::begin_scope()
{
	current->scope_depth += 1;
}

void Compiler::end_scope()
{
	current->scope_depth -= 1;
	//auto local_count = current_function().local_count();

	//while (!current_function().locals.empty() > 0 && current_function().locals.back().depth > current_function().scope_depth)
	//{
	//	emit_byte(OP_POP);
	//	current_function().locals.pop_back();
	//}
	while (!current->locals.empty() && (current->locals.back().depth > current->scope_depth))
	{
		emit_byte(OP_POP);
		current->locals.pop_back();
	}
}

/**
 * @brief returns true if currently compiling for the global scope
*/
bool Compiler::is_global()
{
	return current->scope_depth == 0;
}
}
