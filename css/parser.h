#pragma once

#include <optional>
#include <string>

#include "stylesheet.h"
#include "scanner.h"
#include "token.h"
#include "value.h"

namespace css
{
class Parser
{
public:
	Parser() = delete;
	static Stylesheet parse(std::string);

private:
	Parser(std::string);
	Scanner scanner;
	Token current_token;
	Token previous_token;

	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);
	TokenType peek();

	Stylesheet parse_stylesheet();
	std::optional<Rule> parse_rule();
	std::optional<Selector> parse_selector();
	std::optional<Declaration> parse_declaration();
	Value *parse_value();
};
};
