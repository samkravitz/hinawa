#pragma once

#include <memory>
#include <string>

#include "stylesheet.h"
#include "scanner.h"
#include "token.h"

namespace css
{
class Parser
{
public:
	Parser(std::string);

	std::shared_ptr<Stylesheet> parse();

private:
	Scanner scanner;
	Token current_token;
	Token previous_token;

	void advance();
	void consume(TokenType, const char *);
	bool match(TokenType);

	std::shared_ptr<Stylesheet> parse_stylesheet();
	std::shared_ptr<Rule> parse_rule();
	std::shared_ptr<Selector> parse_selector();
	std::shared_ptr<Declaration> parse_declaration();
};
};
