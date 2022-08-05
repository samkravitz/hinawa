#pragma once

#include <memory>
#include <string>
#include <FlexLexer.h>

#include "stylesheet.h"

namespace css
{
class Parser
{
public:
	Parser(std::string);
	~Parser();

	std::shared_ptr<Stylesheet> parse();

private:
	FlexLexer *lexer;
	std::string input;

	std::shared_ptr<Stylesheet> parse_stylesheet();
	std::shared_ptr<Rule> parse_rule();
	std::shared_ptr<Selector> parse_selector();
	std::shared_ptr<Declaration> parse_declaration();
};
};
