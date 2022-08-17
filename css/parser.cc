#include "parser.h"

#include <cassert>
#include <iostream>
#include <sstream>

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

	while (peek() != OPEN_BRACE)
	{
		consume(COMMA, "expected ','");
		selector = parse_selector();
		if (!selector)
			std::cout << "expected selector\n";

		rule->selectors.push_back(*selector);
	}
	
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

	bool is_classname = false;

	if (current_token.value() == ".")
	{
		is_classname = true;
		advance();
	}

	if (current_token.type() == IDENT)
	{
		selector = std::make_shared<Selector>();

		auto identifier = current_token.value();

		if (is_classname)
			selector->class_name = identifier;		
		else
			selector->tag_name = identifier;

		advance();
	}

	return selector;
}

std::shared_ptr<Declaration> Parser::parse_declaration()
{
	consume(IDENT, "expected identifier");
	auto name = previous_token.value();
	consume(COLON, "expected ':'");
	auto value = parse_value();
	consume(SEMICOLON, "expected ;");
	
	auto declaration = std::make_shared<Declaration>();
	declaration->name = name;
	declaration->value = value;
	return declaration;
}

std::shared_ptr<Value> Parser::parse_value()
{
	std::shared_ptr<Value> value = nullptr;

	switch (current_token.type())
	{
		case HASH:
		{
			auto hex = current_token.value();
			auto color = Color{};
			color.r = std::stoul(hex.substr(1, 2), nullptr, 16);
			color.g = std::stoul(hex.substr(3, 2), nullptr, 16);
			color.b = std::stoul(hex.substr(5, 2), nullptr, 16);
			value = std::make_shared<Color>(color);
			advance();
			break;
		}

		case IDENT:
		{
			auto keyword = Keyword{};
			keyword.value = current_token.value();
			value = std::make_shared<Keyword>(keyword);
			advance();
			break;
		}

		default:
			assert(!"Error parsing a CSS value");
	}

	return value;
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

TokenType Parser::peek()
{
	return current_token.type();
}
}
