#include "parser.h"

#include <iostream>

namespace css
{
Parser::Parser(std::string input) :
	scanner(input.c_str())
{
	advance();
}

std::shared_ptr<Stylesheet> Parser::parse()
{
	return parse_stylesheet();
}

std::shared_ptr<Stylesheet> Parser::parse_stylesheet()
{
	auto stylesheet = std::make_shared<Stylesheet>();
	std::shared_ptr<Rule> rule = nullptr;

	while ((rule = parse_rule()))
	{
		stylesheet->rules.push_back(*rule);
	}

	return stylesheet;
}

std::shared_ptr<Rule> Parser::parse_rule()
{
	auto rule = std::make_shared<Rule>();
	auto selector = parse_selector();

	if (!selector)
		return nullptr;
	
	rule->selectors.push_back(*selector);
	
	if (match(OPEN_BRACE))
	{
		do
		{
			auto declaration = parse_declaration();
			rule->declarations.push_back(*declaration);
		} while (!match(CLOSE_BRACE));
	}
	
	return rule;
}

std::shared_ptr<Selector> Parser::parse_selector()
{
	std::shared_ptr<Selector> selector = nullptr;

	if (current_token.type() == IDENT)
	{
		selector = std::make_shared<Selector>();
		selector->tag_name = current_token.value();
		advance();
	}

	return selector;
}

std::shared_ptr<Declaration> Parser::parse_declaration()
{
	consume(IDENT, "expected identifier");
	auto name = previous_token.value();
	consume(COLON, "expected ':'");
	consume(IDENT, "expected identifier");
	auto value = previous_token.value();
	consume(SEMICOLON, "expected ;");
	
	auto declaration = std::make_shared<Declaration>();
	declaration->name = name;
	declaration->value = value;
	return declaration;
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

void Parser::consume(TokenType type, const char *msg)
{
	if (!match(type))
	{
		std::cout << msg << "\n";
	}
}
}
